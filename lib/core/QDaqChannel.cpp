#include "QDaqChannel.h"

#include <muParser.h>

#include "QDaqEnumHelper.h"
Q_SCRIPT_ENUM(AveragingType,QDaqChannel)
Q_SCRIPT_ENUM(NumberFormat,QDaqChannel)

QDaqChannel::QDaqChannel(const QString& name) :
    QDaqJob(name),
    type_(None),
    fmt_(General),
    digits_(6),
    v_(0.), dv_(0.),
    offset_(0.), multiplier_(1.),
    parser_(0),
    dataReady_(false),
    counter_(0)
{
    range_ << -1e30 << 1.e30;
    ff_ = 0.;
    setForgettingFactor(0.99);
    depth_ = 1;
    buff_.alloc(1);
}

QDaqChannel::~QDaqChannel(void)
{
	if (parser_) delete parser_;
}

void QDaqChannel::detach()
{
    QDaqJob::detach();
}

void QDaqChannel::registerTypes(QScriptEngine* e)
{
	qScriptRegisterAveragingType(e);
	qScriptRegisterNumberFormat(e);
    QDaqJob::registerTypes(e);
}
void QDaqChannel::setSignalName(QString v)
{
	signalName_ = v;
	emit propertiesChanged();
}
void QDaqChannel::setUnit(QString v)
{
	unit_ = v;
	emit propertiesChanged();
}
void QDaqChannel::setRange(const QDaqVector &v)
{
    if((v!=range_) && (!v.isEmpty()) && (v.size()==2) && __finite(v[0]) && __finite(v[1]) && (v[1]!=v[0]))
	{
		// fix ordering
        QDaqVector myv ( v );
		if (v[1]<v[0]) { myv[0] = v[1]; myv[1] = v[0]; }
		// set the range
        os::auto_lock L(comm_lock);
		range_ = myv;
		emit propertiesChanged();
	}
}
void QDaqChannel::setOffset(double v)
{
    os::auto_lock L(comm_lock);
	offset_ = v;
}
void QDaqChannel::setMultiplier(double v)
{
    os::auto_lock L(comm_lock);
	multiplier_ = v;
}
void QDaqChannel::setAveraging(AveragingType t)
{
	//if (throwIfArmed()) return;
	if ((int)t==-1)
	{
		QString msg(
			"Invalid averaging specification. Availiable options: "
			"None, Running, Delta, ForgettingFactor"
			);
		throwScriptError(msg);
		return;
	}
	if (type_ != t)
	{
		{
            os::auto_lock L(comm_lock);
			type_ = t;
		}
		emit propertiesChanged();
	}
}
void QDaqChannel::setDepth(uint d)
{
	if ((d!=depth_) && d>0)
	{
		{
            os::auto_lock L(comm_lock);
			depth_ = d;
			buff_.alloc(d);
			ffw_ = 1. / (1. - pow(ff_,(int)d));
		}
		emit propertiesChanged();
	}
}
bool QDaqChannel::arm_()
{
	counter_ = 0;
	dataReady_ = false;
    return QDaqJob::arm_();
}
bool QDaqChannel::average()
{
	int m = (counter_ < depth_) ? counter_ : depth_;
	v_ = dv_ = 0;

	if (m==0) return false;

	if (type_==None || m==1)
	{
		v_ = buff_[0];
		return true;
	}

	double wt;
	int sw;

	switch(type_)
	{
	case Running:
		for(int i=0; i<m; ++i)
		{
			double y = buff_[i];
			v_ += y;
			dv_ += y*y;
		}
		v_ /= m;
		dv_ /= m;
		break;
	case Delta:
		sw = ((counter_ & 1) == 1) ? -1 : 1; // get the current sign
		for(int i=1; i<m; ++i)
		{
			double y = (buff_[i]-buff_[i-1]);
			v_ += y*sw;
			dv_ += y*y;
			sw = - sw;
		}
		v_  /= (2*(m-1));
		dv_ /= (4*(m-1));
		break;
	case ForgettingFactor:
		wt = 1-ff_; // weight factor
		for(int i=0; i<m; ++i)
		{
			double y = buff_[i];
			v_ += y*wt;
			dv_ += y*y*wt;
			wt *= ff_;
		}
		v_ *= ffw_;
		dv_ *= ffw_;
		break;
    case None:
        break;
	}

	// dv now contains <y^2>
	// convert to st. deviation of <y> = sqrt(<y^2>-<y>^2)
	dv_ -= v_*v_;
	if (dv_>0) dv_=sqrt(dv_);
	else dv_ = 0;

	return true;
}

bool QDaqChannel::run()
{
    if ((dataReady_ = average()))
	{
		if (parser_)
		{
			try
			{
			  v_ = parser_->Eval();
			}
			catch (mu::Parser::exception_type &e)
			{
				//Q_UNUSED(e);
				const mu::string_type& msg = e.GetMsg();
				pushError(QString("muParser"),QString(msg.c_str()));
				//std::cout << e.GetMsg() << endl;
				dataReady_ = false;
			}
		}

		{
			v_ = multiplier_*v_ + offset_;
			dv_ = multiplier_*dv_ + offset_;
		}
		// check limits
        dataReady_ = dataReady_ && __finite(v_) && (v_>range_[0]) && (v_<range_[1]);
        emit updateWidgets();
	}

    return QDaqJob::run();
}
QString QDaqChannel::formatedValue()
{
	QString ret;
	switch(fmt_)
	{
	case General:
		return QString::number(v_,'g',digits_);
	case FixedPoint:
		return QString::number(v_,'f',digits_);
	case Scientific:
		return QString::number(v_,'e',digits_);
	case Time:
        return QDaqTimeValue(v_).toString();
	}

	return QString::number(v_);
//	return dataReady_ ?
//		(time_channel_ ? RtTimeValue(v_).toString() : QString::number(v_)) : QString();
}

void QDaqChannel::clear()
{
    os::auto_lock L(comm_lock);
	counter_ = 0;
	dataReady_ = false;
}


QString QDaqChannel::parserExpression() const
{
	if (parser_) return QString(parser_->GetExpr().c_str());
	else return QString();
}
void QDaqChannel::setForgettingFactor(double v)
{
	if (v!=ff_ && v>0. && v<1.)
	{
        os::auto_lock L(comm_lock);
		ff_ = v;
		ffw_ = 1./(1. - pow(ff_,(int)depth_));
		emit propertiesChanged();
	}
}
void QDaqChannel::setParserExpression(const QString& s)
{
	if (s!=parserExpression())
	{
        os::auto_lock L(comm_lock);

		if (s.isEmpty())
		{
			if (parser_) delete parser_;
			parser_ = 0;
		}
		else
		{
			if (!parser_)
			{
				parser_= new mu::Parser();
				parser_->DefineVar("x",&v_);
			}
			parser_->SetExpr(s.toStdString());
		}

		emit propertiesChanged();
	}
}

/////////////////// QDaqTimeChannel //////////////////////////////////////
QDaqTimeChannel::QDaqTimeChannel(const QString& name) :
    QDaqChannel(name)
{
    dv_ = 0.001;
}

QDaqTimeChannel::~QDaqTimeChannel(void)
{
}

bool QDaqTimeChannel::run()
{
    // disabling averaging etc.
    v_ = QDaqTimeValue::now();
    // time data is always availiable
    dataReady_ = true;

    emit updateWidgets();

    // do not call QDaqChannel::run
    return QDaqJob::run();
}

/////////////////////// QDaqTestChannel //////////////////////////////////////

Q_SCRIPT_ENUM(TestType,QDaqTestChannel)

QDaqTestChannel::QDaqTestChannel(const QString& name) :
    QDaqChannel(name), type_(Random), par_(1.), v(0)
{
}
QDaqTestChannel::~QDaqTestChannel(void)
{
}
void QDaqTestChannel::registerTypes(QScriptEngine* e)
{
    qScriptRegisterTestType(e);
    QDaqChannel::registerTypes(e);
}
bool QDaqTestChannel::run()
{
    //static int i;

    double t;

    switch (type_)
    {
    case Random:
        v = par_*rand()/RAND_MAX;
        break;
    case Inc:
        v += par_;
        break;
    case Dec:
        v -= par_;
        break;
    case Sin:
        t = QDaqTimeValue::now();
        v = sin(6.283185307*par_*t);
    }

    push(v);
    return QDaqChannel::run();
}
//////////////////// QDaqFilterChannel /////////////////////////////
QDaqFilterChannel::QDaqFilterChannel(const QString& name) : QDaqChannel(name)
{
}
void QDaqFilterChannel::setInputChannel(QDaqObject* obj)
{
    QDaqChannel* ch = qobject_cast<QDaqChannel*>(obj);
    if (ch)
    {
        os::auto_lock L(comm_lock);
        inputChannel_ = ch;
        emit propertiesChanged();
    }
}
QDaqObject* QDaqFilterChannel::inputChannel()
{
    if (inputChannel_) return inputChannel_;
    else if (qobject_cast<QDaqChannel*>(parent())) return (QDaqObject*)parent();
    else return 0;
}
bool QDaqFilterChannel::run()
{
    QDaqChannel* ch = (QDaqChannel*)inputChannel();
    if (ch && ch->dataReady()) push(ch->value());
    else push(0);
    return QDaqChannel::run();
}
