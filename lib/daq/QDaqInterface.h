#ifndef _QDAQINTERFACE_H_
#define _QDAQINTERFACE_H_

#include "QDaqObject.h"

#include <QVector>
#include <QHostAddress>
#include <QSerialPort>

class QDaqDevice;

class tcp_socket;

class QDaqInterface : public QDaqObject
{
	Q_OBJECT
	Q_PROPERTY(uint address READ address WRITE setAddress)
	Q_PROPERTY(uint timeout READ timeout WRITE setTimeout)
	Q_PROPERTY(bool isOpen READ isOpen)

protected:
	uint addr_, 
		timeout_; // ms
	bool isOpen_;

protected:
    QVector<QDaqDevice*> ports_;

public:
    explicit QDaqInterface(const QString& name);
    virtual ~QDaqInterface(void);
	virtual void detach();

	// getters
	uint address() const { return addr_; }
	uint timeout() const { return timeout_; }
	bool isOpen() const { return isOpen_; }

	// setters
	void setAddress(uint v);
	void setTimeout(uint v);

	// io
    virtual bool open_port(uint i, QDaqDevice*);
	virtual void close_port(uint i);
	virtual void clear_port(uint i) { i=i; }
	virtual int read(uint port, char* buff, int len, int eos = 0);
	virtual int write(uint port, const char* buff, int len, int eos = 0);
	virtual bool isValidPort(uint i) { i=i; return true; }

	// diagnostics
	virtual int readStatusByte(uint port) { port=port; return 0; }

protected:
	virtual bool open_() { return isOpen_ = true; }
	virtual void close_();
	virtual void clear_() {}
	virtual void setTimeout_(uint v) { timeout_=v; }

	bool throwIfOpen();

public: // slots
	Q_SLOT bool open() { return open_(); } 
	Q_SLOT void close() { close_(); }
	Q_SLOT void clear() { clear_(); }
};



class QDaqTcpip : public QDaqInterface
{
	Q_OBJECT
	Q_PROPERTY(uint port READ port WRITE setPort)
	Q_PROPERTY(QString host READ host WRITE setHost)

protected:
	uint port_;
	QHostAddress host_;
    tcp_socket* socket_;
public:
    Q_INVOKABLE explicit QDaqTcpip(const QString& name, const QString& ahost = QString(), uint portno = 0);
    virtual ~QDaqTcpip();

	// getters
	uint port() const { return port_; }
	QString host() const { return host_.toString(); }

	// setters
	void setPort(uint p);
	void setHost(const QString& h);

	virtual bool isValidPort(uint i) { return i==0; }

	// io
	virtual int read(uint port, char* buff, int len, int eos = 0);
	virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
	virtual bool open_();
	virtual void close_();
};

class QDaqSerial : public QDaqInterface
{
    Q_OBJECT

    Q_PROPERTY(QString portName READ portName WRITE setPortName)
    Q_PROPERTY(QSerialPort::BaudRate baud READ baud WRITE setBaud)
    Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity)
    Q_PROPERTY(QSerialPort::DataBits databits READ databits WRITE setDatabits)
    Q_PROPERTY(QSerialPort::StopBits stopbits READ stopbits WRITE setStopbits)
    Q_PROPERTY(QSerialPort::FlowControl handshake READ handshake WRITE setHandshake)

protected:
    QSerialPort* port_;

public:
    Q_INVOKABLE explicit QDaqSerial(const QString& name, const QString& portName = QString());

    virtual void registerTypes(QScriptEngine* e);

    // getters
    QString portName() const { return port_->portName(); }
    QSerialPort::BaudRate baud() const { return (QSerialPort::BaudRate)port_->baudRate(); }
    QSerialPort::Parity parity() const { return port_->parity(); }
    QSerialPort::DataBits databits() const { return port_->dataBits(); }
    QSerialPort::StopBits stopbits() const { return port_->stopBits(); }
    QSerialPort::FlowControl handshake() const { return port_->flowControl(); }

    // setters
    void setPortName(const QString& aname);
    void setBaud(QSerialPort::BaudRate v);
    void setParity(QSerialPort::Parity v);
    void setDatabits(QSerialPort::DataBits v);
    void setStopbits(QSerialPort::StopBits v);
    void setHandshake(QSerialPort::FlowControl v);

    // io
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
    virtual bool open_();
    virtual void close_();
    virtual void clear_();
};

class QDaqModbusTcp : public QDaqTcpip
{
    Q_OBJECT

protected:
    void* ctx_;

public:
    Q_INVOKABLE explicit QDaqModbusTcp(const QString& name, const QString& ahost = QString(), uint portno = 0);
    virtual ~QDaqModbusTcp();

    // io
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
    virtual bool open_();
    virtual void close_();
};

class QDaqModbusRtu : public QDaqSerial
{
    Q_OBJECT

protected:
    void* ctx_;

public:
    Q_INVOKABLE explicit QDaqModbusRtu(const QString& name, const QString& portName = QString());
    virtual ~QDaqModbusRtu();

    // io
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
    virtual bool open_();
    virtual void close_();
};



#endif


