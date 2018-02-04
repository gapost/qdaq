#include "QDaqEnumHelper.h"
#include "QDaqInterface.h"
#include "QDaqDevice.h"

#include "tcp_socket.h"

#include <modbus.h>

//#elif __linux__
#include <errno.h>
//#endif

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
        if (dev!=0) dev->forcedOffline(QString("Interface %1 closed").arg(fullName()));
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

////////////////// QDaqSerial //////////////////////

Q_SCRIPT_ENUM(BaudRate, QSerialPort)
Q_SCRIPT_ENUM(Parity, QSerialPort)
Q_SCRIPT_ENUM(DataBits, QSerialPort)
Q_SCRIPT_ENUM(StopBits, QSerialPort)
Q_SCRIPT_ENUM(FlowControl, QSerialPort)

QDaqSerial::QDaqSerial(const QString &name, const QString &portName) :
    QDaqInterface(name)
{
    port_ = new QSerialPort(portName,this); // QextSerialPort::Polling,

    ports_.push_back((QDaqDevice*)0); // only 1 device

}

void QDaqSerial::registerTypes(QScriptEngine* e)
{
    qScriptRegisterBaudRate(e);
    qScriptRegisterParity(e);
    qScriptRegisterDataBits(e);
    qScriptRegisterStopBits(e);
    qScriptRegisterFlowControl(e);
    QDaqInterface::registerTypes(e);
}

void QDaqSerial::setPortName(const QString &aname)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setPortName(aname);
        emit propertiesChanged();
    }
}

void QDaqSerial::setBaud(QSerialPort::BaudRate v)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setBaudRate(v);
        emit propertiesChanged();
    }
}
void QDaqSerial::setParity(QSerialPort::Parity v)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setParity(v);
        emit propertiesChanged();
    }
}
void QDaqSerial::setDatabits(QSerialPort::DataBits v)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setDataBits(v);
        emit propertiesChanged();
    }
}
void QDaqSerial::setStopbits(QSerialPort::StopBits v)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setStopBits(v);
        emit propertiesChanged();
    }
}
void QDaqSerial::setHandshake(QSerialPort::FlowControl v)
{
    if (throwIfOpen()) return;
    else
    {
        port_->setFlowControl(v);
        emit propertiesChanged();
    }
}

int QDaqSerial::read(uint port, char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    port=port;

    if (!port_->isOpen()) return 0;

    int read = 0;
    char c;
    char eos_char = eos & 0xFF;
    bool ok;
    while ( (ok = port_->waitForReadyRead(timeout())) )
    {
        port_->read(&c,1);
        buff[read++] = c;
        if (eos_char && c==eos_char)
        {
            read--;
            break;
        }
        if (read==len) break;
    }
    if (!ok)
    {
        pushError("Read char failed", "possibly timed-out");
    }
    return ok ? read : 0;
}

int QDaqSerial::write(uint port, const char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    port=port;

    if (!port_->isOpen()) return 0;

    char eos_char = eos & 0xFF;

    port_->write(buff,len);
    port_->write(&eos_char,1);
    bool ok = port_->waitForBytesWritten(timeout());

    if (!ok)
    {
        pushError("Write timeout");
    }
    return ok ? len : 0;
}

bool QDaqSerial::open_()
{
    if (isOpen()) return true;
    os::auto_lock L(comm_lock);

    if (port_->open(QIODevice::ReadWrite))
        QDaqInterface::open_();
    else pushError(QString("Open %1 failed").arg(portName()));
    emit propertiesChanged();
    return isOpen();
}

void QDaqSerial::close_()
{
    os::auto_lock L(comm_lock);
    QDaqInterface::close_();
    port_->close();
}

void QDaqSerial::clear_()
{
    os::auto_lock L(comm_lock);
    if (port_->isOpen()) port_->flush();
}




QDaqModbusTcp::QDaqModbusTcp(const QString& name, const QString& host, uint portn) :
    QDaqTcpip(name, host, portn), ctx_(0)
{
}

QDaqModbusTcp::~QDaqModbusTcp(void)
{
}

bool QDaqModbusTcp::open_()
{
    if (isOpen() && ctx_) return true;

    os::auto_lock L(comm_lock);

    /* TCP */
    modbus_t* ctx = modbus_new_tcp(host().toLatin1().constData(), port_);


    timeval response_timeout;
    /* Define a new and too short timeout! */
    response_timeout.tv_sec = 0;
    response_timeout.tv_usec = timeout_*1000;
    modbus_set_response_timeout(ctx, &response_timeout);


    if (modbus_connect(ctx) == -1) {
        pushError("modbus_connect failed", modbus_strerror(errno));
        modbus_free(ctx);
        isOpen_ = false;
    }
    else {
        isOpen_ = true;
        ctx_ = ctx;
    }

    emit propertiesChanged();

    return isOpen();
}

void QDaqModbusTcp::close_()
{
    os::auto_lock L(comm_lock);
    QDaqInterface::close_();
    /* Close the connection */
    if (ctx_) {
        modbus_t* ctx = (modbus_t*)ctx_;
        modbus_close(ctx);
        modbus_free(ctx);
        ctx_ = 0;
        emit propertiesChanged();
    }
}

int QDaqModbusTcp::read(uint port, char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    modbus_t* ctx = (modbus_t*)ctx_;

    eos = eos;
    int addr = port;
    uint16_t* regs = (uint16_t*)buff;
    int nb = (len/2) + (len%2);
    int ret = modbus_read_registers(ctx, addr, nb, regs);
    if (ret == -1) {
        pushError("modbus_read_registers failed", modbus_strerror(errno));
        return 0;
    }
    else return len;
}

int QDaqModbusTcp::write(uint port, const char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    modbus_t* ctx = (modbus_t*)ctx_;

    eos = eos;
    int addr = port;
    const uint16_t* regs = (const uint16_t*)buff;
    int nb = (len/2) + (len%2);
    int ret = modbus_write_registers(ctx, addr, nb, regs);
    if (ret == -1) {
        pushError("modbus_write_registers failed", modbus_strerror(errno));
        return 0;
    }
    return len;
}

/**************** QDaqModbusRtu ***********************/

QDaqModbusRtu::QDaqModbusRtu(const QString& name, const QString& portName) :
    QDaqSerial(name, portName), ctx_(0)
{
}

QDaqModbusRtu::~QDaqModbusRtu(void)
{
}

bool QDaqModbusRtu::open_()
{
    if (isOpen() && ctx_) return true;

    os::auto_lock L(comm_lock);

    const char* device = portName().toLatin1().constData();
    int baud_ = (int)(this->baud());
    char par_ = 'N';
    if (parity()==QSerialPort::EvenParity) par_='E';
    else if (parity()==QSerialPort::OddParity) par_='O';

    int stop_ = this->stopbits()==QSerialPort::OneStop ? 1 : 2;

    /* Create RTU module */
    modbus_t* ctx = modbus_new_rtu(device,baud_, par_, (int)(this->databits()), stop_);

    if (modbus_connect(ctx) == -1) {
        pushError("modbus_connect failed", modbus_strerror(errno));
        modbus_free(ctx);
        isOpen_ = false;
    }
    else {
        isOpen_ = true;
        ctx_ = ctx;
        modbus_set_slave(ctx,1);
    }

    emit propertiesChanged();

    return isOpen();
}

void QDaqModbusRtu::close_()
{
    os::auto_lock L(comm_lock);
    QDaqInterface::close_();
    /* Close the connection */
    if (ctx_) {
        modbus_t* ctx = (modbus_t*)ctx_;
        modbus_close(ctx);
        modbus_free(ctx);
        ctx_ = 0;
        emit propertiesChanged();
    }
}

int QDaqModbusRtu::read(uint port, char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    modbus_t* ctx = (modbus_t*)ctx_;

    eos = eos;
    int addr = port;
    uint16_t* regs = (uint16_t*)buff;
    int nb = (len/2) + (len%2);
    int ret = modbus_read_registers(ctx, addr, nb, regs);
    if (ret == -1) {
        pushError("modbus_read_registers failed", modbus_strerror(errno));
        return 0;
    }
    else return len;
}

int QDaqModbusRtu::write(uint port, const char* buff, int len, int eos)
{
    os::auto_lock L(comm_lock);

    modbus_t* ctx = (modbus_t*)ctx_;

    eos = eos;
    int addr = port;
    const uint16_t* regs = (const uint16_t*)buff;
    int nb = (len/2) + (len%2);
    int ret = modbus_write_registers(ctx, addr, nb, regs);
    if (ret == -1) {
        pushError("modbus_write_registers failed", modbus_strerror(errno));
        return 0;
    }
    return len;
}










