#include "RtDataChannel.h"

#include <muParser.h>

#include "RtEnumHelper.h"
Q_SCRIPT_ENUM(AveragingType,RtDataChannel)
Q_SCRIPT_ENUM(NumberFormat,RtDataChannel)

RtDataChannel::RtDataChannel(const QString& name) :
	RtJob(name), type_(None), fmt_(General), digits_(6),
	v_(0.), dv_(0.),
	offset_(0.), multiplier_(1.),
	parser_(0),
	dataReady_(false),
	counter_(0)
{
	range_ << -1e30 << 1.e30;
	setForgettingFactor(0.99);
}

RtDataChannel::~RtDataChannel(void)
{
	if (parser_) delete parser_;
}

void RtDataChannel::detach()
{
	RtJob::detach();
}

void RtDataChannel::registerTypes(QScriptEngine* e)
{
	qScriptRegisterAveragingType(e);
	qScriptRegisterNumberFormat(e);
	RtJob::registerTypes(e);
}
void RtDataChannel::setSignalName(QString v)
{
	signalName_ = v;
	emit propertiesChanged();
}
void RtDataChannel::setUnit(QString v)
{
	unit_ = v;
	emit propertiesChanged();
}
void RtDataChannel::setRange(const RtDoubleVector& v)
{
	if((v!=range_) && (!v.isEmpty()) && (v.size()==2) && _finite(v[0]) && _finite(v[1]) && (v[1]!=v[0]))
	{
		// fix ordering
		RtDoubleVector myv ( v );
		if (v[1]<v[0]) { myv[0] = v[1]; myv[1] = v[0]; }
		// set the range
		auto_lock L(comm_lock);
		range_ = myv;
		emit propertiesChanged();
	}
}
void RtDataChannel::setOffset(double v)
{
	auto_lock L(comm_lock);
	offset_ = v;
}
void RtDataChannel::setMultiplier(double v)
{
	auto_lock L(comm_lock);
	multiplier_ = v;
}
void RtDataChannel::setAveraging(AveragingType t)
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
			auto_lock L(comm_lock);
			type_ = t;
		}
		emit propertiesChanged();
	}
}
void RtDataChannel::setDepth(uint d)
{
	if ((d!=depth_) && d>0)
	{
		{
			auto_lock L(comm_lock);
			depth_ = d;
			buff_.alloc(d);
			ffw_ = 1. / (1. - pow(ff_,(int)d));
		}
		emit propertiesChanged();
	}
}
bool RtDataChannel::arm_()
{
	counter_ = 0;
	dataReady_ = false;
	return RtJob::arm_();
}
bool RtDataChannel::average()
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
	}

	// dv now contains <y^2>
	// convert to st. deviation of <y> = sqrt(<y^2>-<y>^2)
	dv_ -= v_*v_;
	if (dv_>0) dv_=sqrt(dv_);
	else dv_ = 0;

	return true;
}

void RtDataChannel::run()
{
	if (dataReady_ = average())
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
		dataReady_ = dataReady_ && _finite(v_) && (v_>range_[0]) && (v_<range_[1]);
		updateWidgets();
	}
	RtJob::run();
}
QString RtDataChannel::formatedValue()
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
		return RtTimeValue(v_).toString();
	}

	return QString::number(v_);
//	return dataReady_ ?
//		(time_channel_ ? RtTimeValue(v_).toString() : QString::number(v_)) : QString();
}

void RtDataChannel::clear()
{
	auto_lock L(comm_lock);
	counter_ = 0;
	dataReady_ = false;
}


QString RtDataChannel::parserExpression() const
{
	if (parser_) return QString(parser_->GetExpr().c_str());
	else return QString();
}
void RtDataChannel::setForgettingFactor(double v)
{
	if (v!=ff_ && v>0. && v<1.)
	{
		auto_lock L(comm_lock);
		ff_ = v;
		ffw_ = 1./(1. - pow(ff_,(int)depth_));
		emit propertiesChanged();
	}
}
void RtDataChannel::setParserExpression(const QString& s)
{
	if (s!=parserExpression())
	{
		auto_lock L(comm_lock);

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
