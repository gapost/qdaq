#include "qdaqtcpip.h"

#include "tcp_socket.h"

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
    QMutexLocker L(&comm_lock);

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
    QMutexLocker L(&comm_lock);

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

    QMutexLocker L(&comm_lock);

    bool ret = socket_->connect(host_.toString().toLatin1().constData(), port_)!=-1;
    if (ret) QDaqInterface::open_();
    else
        pushError("Socket connect failed",tcp_socket::lastErrorStr());

    emit propertiesChanged();

    return isOpen();
}

void QDaqTcpip::close_()
{
    QMutexLocker L(&comm_lock);
    QDaqInterface::close_();
    socket_->disconnect();
}
