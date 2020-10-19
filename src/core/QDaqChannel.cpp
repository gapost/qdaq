#include "QDaqChannel.h"
#include <QChar>
#include <muParser.h>

#include <algorithm>

QDaqChannel::QDaqChannel(const QString& name) :
    QDaqJob(name),
    channeltype_(Normal),
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
    sorted_buffer.resize(1);
}

QDaqChannel::~QDaqChannel(void)
{
	if (parser_) delete parser_;
}

void QDaqChannel::detach()
{
    QDaqJob::detach();
}

void QDaqChannel::setType(ChannelType t)
{
    //if (throwIfArmed()) return;
    if ((int)t==-1)
    {
        QString msg(
            "Invalid channel type specification. Availiable options: "
            "Normal, Clock, Random, Inc, Dec."
            );
        throwScriptError(msg);
        return;
    }
    if (channeltype_ != t)
    {
        {
            QMutexLocker L(&comm_lock);
            channeltype_ = t;
            if (t==Clock) setFormat(Time);
        }
        emit propertiesChanged();
    }
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
    if((v!=range_) && (!v.isEmpty()) && (v.size()==2) && std::isfinite(v[0]) && std::isfinite(v[1]) && (v[1]!=v[0]))
	{
		// fix ordering
        QDaqVector myv(v);
        if (v[1]<v[0]) {
            myv.clear();
            myv << v[1] << v[0];
        }
		// set the range
        QMutexLocker L(&comm_lock);
		range_ = myv;
		emit propertiesChanged();
	}
}
void QDaqChannel::setOffset(double v)
{
    QMutexLocker L(&comm_lock);
	offset_ = v;
}
void QDaqChannel::setMultiplier(double v)
{
    QMutexLocker L(&comm_lock);
	multiplier_ = v;
}
void QDaqChannel::setAveraging(AveragingType t)
{
	//if (throwIfArmed()) return;
	if ((int)t==-1)
	{
		QString msg(
			"Invalid averaging specification. Availiable options: "
            "None, Running, Delta, ForgettingFactor, Median"
			);
		throwScriptError(msg);
		return;
	}
	if (type_ != t)
	{
		{
            QMutexLocker L(&comm_lock);
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
            QMutexLocker L(&comm_lock);
			depth_ = d;
			buff_.alloc(d);
            sorted_buffer.resize(d);
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
    if (m==0) return false;

	if (type_==None || m==1)
	{
		v_ = buff_[0];
        dv_ = 0;
		return true;
	}

    v_ = dv_ = 0;

	double wt;
    int sw, m2;
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
    case Median:
        for(int i=0; i<m; ++i)
        {
            sorted_buffer[i] = buff_[i];
            double y = buff_[i];
            dv_ += y*y;
        }
        std::sort(sorted_buffer.begin(),sorted_buffer.end()); //needs #include <algorithm>
        m2 = m/2;
        if (m % 2) { v_ = sorted_buffer[m2];}
        else v_ = (sorted_buffer[m2-1] + sorted_buffer[m2])/2.0;
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
    if (!QDaqJob::run()) return false;

    dataReady_ = true;
    switch (channeltype_)
    {
    case Clock:
        push(QDaqTimeValue::now());
        break;
    case Random:
        push(1.*rand()/RAND_MAX);
        break;
    case Inc:
        push(v_ + 1);
        break;
    case Dec:
        push(v_ - 1);
        break;
    case Normal:
    default:
        break;
    }

    if ((dataReady_=average()))
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
#if defined(_UNICODE)
                pushError(QString("muParser"),QString::fromStdWString(msg));
#else
				pushError(QString("muParser"),QString(msg.c_str()));
#endif
				//std::cout << e.GetMsg() << endl;
				dataReady_ = false;
			}
		}

		{
			v_ = multiplier_*v_ + offset_;
			dv_ = multiplier_*dv_ + offset_;
		}

        // handle over/under flow
        if (v_ < range_[0]) v_ = range_[0];
        if (v_ > range_[1]) v_ = range_[1];

        dataReady_ = std::isfinite(v_);

        emit updateWidgets();
	}

    return true;
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
    QMutexLocker L(&comm_lock);
	counter_ = 0;
	dataReady_ = false;
}


QString QDaqChannel::parserExpression() const
{
#if defined(_UNICODE)
    if (parser_) return QString::fromStdWString(parser_->GetExpr());
#else
	if (parser_) return QString(parser_->GetExpr().c_str());
#endif
	else return QString();
}
void QDaqChannel::setForgettingFactor(double v)
{
	if (v!=ff_ && v>0. && v<1.)
	{
        QMutexLocker L(&comm_lock);
		ff_ = v;
		ffw_ = 1./(1. - pow(ff_,(int)depth_));
		emit propertiesChanged();
	}
}
void QDaqChannel::setParserExpression(const QString& s)
{
	if (s!=parserExpression())
	{
        QMutexLocker L(&comm_lock);

		if (s.isEmpty())
		{
			if (parser_) delete parser_;
			parser_ = 0;
		}
		else
		{
#if defined(_UNICODE)
            if (!parser_)
            {
                parser_= new mu::Parser();
                parser_->DefineVar(QString("x").toStdWString(),&v_);
            }
            parser_->SetExpr(s.toStdWString());
#else
			if (!parser_)
			{
				parser_= new mu::Parser();
				parser_->DefineVar("x",&v_);
			}
			parser_->SetExpr(s.toStdString());
#endif
		}

		emit propertiesChanged();
	}
}


