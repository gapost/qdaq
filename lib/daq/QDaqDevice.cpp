#include "QDaqDevice.h"

QDaqDevice::QDaqDevice(const QString& name) :
    QDaqJob(name),
    addr_(0),
    online_(false),
    eot_(0x02 << 8),
    eos_(0x100),
    buff_(4096,char(0)),
    buff_sz_(4096)
{
}
QDaqDevice::~QDaqDevice()
{
}
void QDaqDevice::detach()
{
    QDaqJob::detach();
	setOnline_(false);
}
// on-off
void QDaqDevice::setOnline(bool on)
{
	if (throwIfArmed()) return;
    os::auto_lock L(comm_lock);
	if (on!=online_)
	{
		setOnline_(on);
		emit propertiesChanged();
	}
}
bool QDaqDevice::on()
{
	setOnline(true);
	return online_; 
}
void QDaqDevice::off()
{
	setOnline(false);
} 
bool QDaqDevice::setOnline_(bool on)
{
	if (on==online_) return online_;
	if (on && ifc) 
	{
		// set this device to on
		online_ = ifc->open_port(addr_,this);
		if (online_) ifc->clear_port(addr_);
		// switch-on also the sub-devices
		if (online_)
		{
			bool ok = true;
			int k = 0;
            foreach(QDaqJob* j, subjobs_)
			{
                QDaqDevice* dev = qobject_cast<QDaqDevice*>(j);
				if (dev)
				{
					ok = dev->setOnline_(true);
					if (!ok) break;
					k++;
				}
			}
			if (!ok) // switch off all
			{
				online_ = false;
				ifc->close_port(addr_);
                foreach(QDaqJob* j, subjobs_)
				{
                    QDaqDevice* dev = qobject_cast<QDaqDevice*>(j);
					if (dev)
					{
						ok = dev->setOnline_(false);
						k--;
						if (k==0) break;
					}
				}
			}
		}
		return online_;
	}
	else {  
		if (ifc) ifc->close_port(addr_); // ifc should excist
		return online_ = false;
	}
}
void QDaqDevice::forcedOffline(const QString& reason)
{
    os::auto_lock L(comm_lock);
	if (armed_) disarm_();// forcedDisarm(reason);
	if (online_) 
	{
		setOnline_(false);
		pushError("forced offline", reason);
	}
}
// setters
void QDaqDevice::setBufferSize(unsigned int sz)
{
	if (throwIfArmed()) return;
    os::auto_lock L(comm_lock);
    buff_.resize(sz);
    buff_sz_ = sz;
	emit propertiesChanged();
}
void QDaqDevice::setAddress(int a)
{
	if (throwIfOnline()) return;
    os::auto_lock L(comm_lock);
	addr_ = a;
	emit propertiesChanged();
}
void QDaqDevice::setEot(int e)
{
	if (throwIfArmed()) return;
    os::auto_lock L(comm_lock);
	eot_ = e;
}
void QDaqDevice::setEos(int e)
{
	if (throwIfArmed()) return;
    os::auto_lock L(comm_lock);
	eos_ = e;
}
void QDaqDevice::setInterface(QDaqInterface* i)
{
	if (i==ifc) return;
	if (armed_) disarm_();
	if (online_) setOnline_(false);
	ifc = i;
	emit propertiesChanged();
}
// states / messages
bool QDaqDevice::throwIfOffline()
{
	bool ret = !online_;
	if (ret) throwScriptError("Not possible when device is offline.");
	return ret;
}
bool QDaqDevice::throwIfOnline()
{
	if (online_) throwScriptError("Not possible when device is online.");
	return online_;
}
// arming
bool QDaqDevice::arm_()
{
	if (throwIfOffline()) return armed_ = false;
    else return QDaqJob::arm_();
}
// io
int QDaqDevice::write_(const char* msg, int len)
{
    os::auto_lock L(comm_lock);
	int ret = ifc->write(addr_, msg, len, eot_);
	//if (!ret && armed_) forcedDisarm(QString("Write to device %1 failed").arg(fullName()));
	checkError(msg,len);
	return ret;
}
int QDaqDevice::write_(const char* msg)
{
	return write_(msg, strlen(msg));
}
int QDaqDevice::write_(const QByteArray& msg)
{
	return write_(msg.constData(),msg.size());
}
bool QDaqDevice::write_(const QList<QByteArray>& msglist)
{
    os::auto_lock L(comm_lock);
	foreach(const QByteArray& msg, msglist)
	{
		if (msg.size() != write_(msg)) return false;
	}
	return true;
}
int QDaqDevice::write(const QString& msg)
{
	//if (throwIfArmed()) return 0;
	if (throwIfOffline()) return 0;
	return write_(msg.toLatin1());
}
int QDaqDevice::write(int reg, int val) // write
{
    if (throwIfOffline()) return 0;
    os::auto_lock L(comm_lock);
    unsigned short b = val;
    int ret = ifc->write(reg, (const char *)(&b), sizeof(b), eot_);


    return ret;
}

QByteArray QDaqDevice::read_()
{
    os::auto_lock L(comm_lock);
    buff_.resize(buff_sz_);
    char* mem = buff_.data();
    int cnt =  ifc->read(addr_,mem,buff_sz_,eos_);
    buff_.resize(cnt);
	//if (!buff_cnt_ && armed_) forcedDisarm(QString("Read from device %1 failed").arg(fullName())); 
    return buff_;
}
QString QDaqDevice::read()
{
	//if (throwIfArmed()) return QString();
	if (throwIfOffline()) return QString();
	return QString(read_());
}
int QDaqDevice::read(int reg) // read register from device (modbus type)
{
    if (throwIfOffline()) return 0;

    os::auto_lock L(comm_lock);

    unsigned short b = 0;
    ifc->read(reg,(char *)(&b),sizeof(b),eos_);

    return b;
}
QDaqIntVector QDaqDevice::read(int reg, int n) // read n consequtive registers from device (modbus type)
{

    if (throwIfOffline()) return QDaqIntVector();

    os::auto_lock L(comm_lock);


    buff_.resize(2*n);
    char* mem = buff_.data();
    int cnt = ifc->read(reg,mem,2*n,eos_);

    if (cnt) {
        QDaqIntVector V(n);
        unsigned short* b = (unsigned short*)mem;
        for(int i=0; i<n; i++) V[i] = b[i];
        return V;
    }
    else return QDaqIntVector();
}

QString QDaqDevice::query(const QString& msg)
{
	//if (throwIfArmed()) return QString();
	if (throwIfOffline()) return QString();
    os::auto_lock L(comm_lock);
	write_(msg.toLatin1());
	return QString(read_());
}
int QDaqDevice::statusByte_()
{
	return ifc->readStatusByte(addr_);
}
int QDaqDevice::statusByte()
{
	if (throwIfArmed()) return 0;
	if (throwIfOffline()) return 0;
	return statusByte_();
}
void QDaqDevice::clear()
{
	if (throwIfArmed()) return;
	if (throwIfOffline()) return;
	ifc->clear_port(addr_);
}
