#include "QDaqInterface.h"
//#include "QDaqDevice.h"
#include "QDaqEnumHelper.h"

#include "tcp_socket.h"

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
	os::auto_lock L(comm_lock);
	if (!isValidPort(i) || ports_[i]!=0) return false;
	else { ports_[i] = dev; return true; }
}

void QDaqInterface::close_port(uint i)
{
	os::auto_lock L(comm_lock);
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
	os::auto_lock L(comm_lock);
	if (!isOpen()) return;
	for(int i=0; i<ports_.size(); ++i)
	{
        QDaqDevice* dev = ports_[i];
        //TODO
        //if (dev!=0) dev->forcedOffline(QString("Interface %1 closed").arg(fullName()));
		ports_[i] = 0;
	}
	isOpen_ = false;
	emit propertiesChanged();
}



//**********************************************************************//

QDaqTcpip::QDaqTcpip(const QString& name, const QString& host, uint portn) :

    QDaqInterface(name),
    port_(portn),
    host_(host)
{
    socket_ = new tcp_socket;
	// only one port
    ports_.push_back((QDaqDevice*)0);
}

QDaqTcpip::~QDaqTcpip(void)
{
}

void QDaqTcpip::setPort(uint p)
{
	if (throwIfOpen()) return;
	else
	{
		port_ = p;
		emit propertiesChanged();
	}
}
void QDaqTcpip::setHost(const QString& h)
{
	if (throwIfOpen()) return;
	else
	{
        if (host_.setAddress(h))
            emit propertiesChanged();
        else throwScriptError("Invalid IP address.");
	}
}

int QDaqTcpip::read(uint port, char* buff, int len, int eos)
{
	os::auto_lock L(comm_lock);

	port=port;
	eos=eos;

    if (!socket_->is_connected()) return 0;

    int rc = socket_->receive(buff,len);
    if (rc==-1){
        pushError("Socket recieve error.",tcp_socket::lastErrorStr());
        return 0;
    }

    return rc;
}

int QDaqTcpip::write(uint port, const char* buff, int len, int eos)
{
	os::auto_lock L(comm_lock);

	port=port;
	eos=eos;

    if (!socket_->is_connected()) return 0;

    int w = socket_->send(buff,len);
    if (w==-1)
	{
        pushError("Socket send error.",tcp_socket::lastErrorStr());
		return 0;
	}

    return w;
}

bool QDaqTcpip::open_()
{
    if (isOpen() && socket_->is_connected()) return true;

	os::auto_lock L(comm_lock);

    isOpen_ = socket_->connect(host_.toString().toLatin1().constData(), port_)!=-1;
    if (!isOpen_)
        pushError("Socket connect failed",tcp_socket::lastErrorStr());

	emit propertiesChanged();

	return isOpen();
}

void QDaqTcpip::close_()
{
	os::auto_lock L(comm_lock);
    QDaqInterface::close_();
    socket_->disconnect();
}








