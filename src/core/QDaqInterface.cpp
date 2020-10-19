#include "QDaqInterface.h"
#include "QDaqDevice.h"

QDaqInterface::QDaqInterface(const QString& name) :
QDaqObject(name), addr_(0), timeout_(300), isOpen_(false)
{
}

QDaqInterface::~QDaqInterface(void)
{
}
void QDaqInterface::detach()
{
	close_();
    QDaqObject::detach();
}

void QDaqInterface::setAddress(uint v)
{
	if (throwIfOpen()) return;
	else
	{
		addr_ = v;
		emit propertiesChanged();
	}
}
void QDaqInterface::setTimeout(uint v)
{
	if (throwIfOpen()) return;
	else
	{
		setTimeout_(v);
        timeout_ = v;
		emit propertiesChanged();
	}
}

bool QDaqInterface::throwIfOpen()
{
	if (isOpen_) throwScriptError("Not possible while interface is open");
	return isOpen_;
}

bool QDaqInterface::open_port(uint i, QDaqDevice* dev)
{
	if (!isOpen_) return false;
	QMutexLocker L(&comm_lock);
	if (!isValidPort(i) || ports_[i]!=0) return false;
	else { ports_[i] = dev; return true; }
}

void QDaqInterface::close_port(uint i)
{
	QMutexLocker L(&comm_lock);
	if (isValidPort(i)) ports_[i]=0;
}

int QDaqInterface::read(uint port, char* buff, int len, int eos)
{
	port=port;
	buff=buff;
	len=len;
	eos=eos;
	return 0;
}

int QDaqInterface::write(uint port, const char* buff, int len, int eos)
{
    port=port;
    port=buff[0];
	len=len;
	eos=eos;
	return 0;
}

void QDaqInterface::close_()
{
	QMutexLocker L(&comm_lock);
	if (!isOpen()) return;
	for(int i=0; i<ports_.size(); ++i)
	{
        QDaqDevice* dev = ports_[i];
        //TODO
        if (dev!=0) dev->forcedOffline(QString("Interface %1 closed").arg(path()));
		ports_[i] = 0;
	}
	isOpen_ = false;
	emit propertiesChanged();
}

