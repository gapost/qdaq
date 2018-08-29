#ifndef _QDAQINTERFACE_H_
#define _QDAQINTERFACE_H_

#include "QDaqObject.h"

#include <QVector>
#include <QHostAddress>
#include <QSerialPort>

class QDaqDevice;

class tcp_socket;

/**
 * @brief Base class of objects representing digital communications interfaces.
 *
 * @ingroup Daq
 *
 * QDaqInterface provides a common implementation for communication interfaces
 * as, e.g., serial (RS232,RS485,USB), ethernet (TCP/IP), MODBUS and GPIB.
 *
 * The QDaqInstrument class utilizes a descendant of QDaqInterface
 * for performing the actual communications with the instrument.
 *
 * A QDaqInterface has a number of communication "ports", which have different
 * meaning depending on the specific interface implementation.
 * On memory/register based interfaces like MODBUS, a port represents the
 * register number to be read/written. On interfaces that support connection
 * with multiple instruments (GPIB, RS485), the port is essentially the address of each
 * instrument.
 * On interfaces that support only
 * one-to-one communication with an instrument (RS232, TCP/IP) the port is not used.
 *
 */
class QDAQ_EXPORT QDaqInterface : public QDaqObject
{
	Q_OBJECT

    /** Address of this interface.
     *
     * The meaning of address depends on the type of interface.
     *
     * The address can be changed when the interface is closed.
     *
     */
	Q_PROPERTY(uint address READ address WRITE setAddress)
    /// Communications timeout in ms.
	Q_PROPERTY(uint timeout READ timeout WRITE setTimeout)
    /// Returns true if the interface is open.
	Q_PROPERTY(bool isOpen READ isOpen)

protected:
    // properties
	uint addr_, 
		timeout_; // ms
	bool isOpen_;

    // devices connected at the "ports" of this interface
    QVector<QDaqDevice*> ports_;

public:
    /// Construct a QDaqInterface with object-name name.
    explicit QDaqInterface(const QString& name);
    virtual ~QDaqInterface(void);

    /// When a QDaqInterface is detached it is also closed.
	virtual void detach();

	// getters
	uint address() const { return addr_; }
	uint timeout() const { return timeout_; }
	bool isOpen() const { return isOpen_; }

	// setters
	void setAddress(uint v);
    void setTimeout(uint v);

	// io
    /**
     * @brief Open communication port no. i for QDaqDevice d
     *
     * Returns true if the port was succesfully opened.
     */
    virtual bool open_port(uint i, QDaqDevice* d);
    /// Closes port i.
	virtual void close_port(uint i);
    /// Clear the communication channel with port i (interface specific).
	virtual void clear_port(uint i) { i=i; }
    /**
     * @brief Read data from a port.
     *
     * The meaning of the port parameter is different depending on
     * the type of communication interface.
     *
     * In message based communications (e.g. GPIB) the port is the
     * instrument address.
     *
     * In memory based interfaces (e.g. MODBUS) the port number
     * represents the starting register address to access. The number of
     * registers read/written depends on len, the size of the buffer in bytes.
     * A MODBUS register is 16 bit, ie, occupies 2 bytes.
     *
     * @param port Port number.
     * @param buff Pointer to memory buffer.
     * @param len Allocated number of bytes.
     * @param eos End of string byte
     * @return Number of bytes read.
     */
    virtual int read(uint port, char* buff, int len, int eos = 0);
    /**
     * @brief Write data to a port.
     *
     * The meaning of the port parameter is different depending on
     * the type of communication interface.
     *
     * In message based communications (e.g. GPIB) the port is the
     * instrument address.
     *
     * In memory based interfaces (e.g. MODBUS) the port number
     * represents the starting register address to access. The number of
     * registers read/written depends on len, the size of the buffer in bytes.
     * A MODBUS register is 16 bit, ie, occupies 2 bytes.
     *
     * @param p Port number.
     * @param buff Pointer to memory buffer.
     * @param len Number of bytes to write.
     * @param eos End of string byte
     * @return Number of bytes written.
     */
    virtual int write(uint port, const char* buff, int len, int eos = 0);
    /// Returns true is i is a valid port number.
	virtual bool isValidPort(uint i) { i=i; return true; }
    /// Returns a status byte for a port (interface specific).
	virtual int readStatusByte(uint port) { port=port; return 0; }
    /// Trigger a device connected to port (interface specific).
    virtual void trigger(uint port) { port=port; }

protected:
    /// Implementation of interface open.
	virtual bool open_() { return isOpen_ = true; }
    /// Implementation of interface close.
	virtual void close_();
    /// Implementation of interface clear.
	virtual void clear_() {}
    /// Implementation of setting the timeout.
    virtual void setTimeout_(uint v) { timeout_=v; }
    /// Throws a script error and a QDaqError if called on an open interface.
	bool throwIfOpen();

public slots:
    /// Opens the interface and returns true if succesful.
    bool open() { return open_(); }
    /// Closes the interface.
    void close() { close_(); }
    /// Clears the interface.
    void clear() { clear_(); }
};


/**
 * @brief A class for accesing a TCP/IP interface.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * QDaqTcpip performs communications with another host
 * using a tcp socket connection
 * to the specified host address and port. The port here
 * means the tcp/ip port and is not to be confused with
 * the ports of QDaqInterface.
 *
 * QDaqTcpip can be used in order to communicate with a single
 * device. For multiple devices connected on the ethernet bus, each with its
 * own host ip address,
 * a separate QDaqTcpip must be used per device.
 *
 * If the same device with single ip address offers several ports
 * for communication, then a separate QDaqTcpip must be used per
 * device port.
 *
 */
class QDAQ_EXPORT QDaqTcpip : public QDaqInterface
{
	Q_OBJECT

    /// Tcp/ip port.
	Q_PROPERTY(uint port READ port WRITE setPort)
    /// Host IP address as string, eg '192.168.1.6'
	Q_PROPERTY(QString host READ host WRITE setHost)

protected:
    // properties
	uint port_;
	QHostAddress host_;
    // my socket
    tcp_socket* socket_;
public:

    /**
     * @brief QDaqTcpip constructor.
     *
     * It initializes the object with the interface is in the closed state.
     *
     * @param name QDaqObject name
     * @param ahost Host address
     * @param portno Port number where the host listens
     */
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
/**
 * @brief A class for accesing a serial bus.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * QDaqSerial performs communications with devices connected
 * to a serial port. It is essentially a wrapper for the QSerialPort class.
 * Communications are performed in blocking mode, i.e., QDaq will wait for
 * a message to be read/written to the serial port before it continues.
 * This is because interfaces will be generally used in a QDaqLoop that runs
 * in a separate thread and in most cases the information that must be transmitted/obtained
 * from the serial port is essential for the loop to continue.
 *
 *
 */
class QDAQ_EXPORT QDaqSerial : public QDaqInterface
{
    Q_OBJECT

    /// Name of the serial port, eg, 'COM1' in windows, '/dev/ttyS0' in linux.
    Q_PROPERTY(QString portName READ portName WRITE setPortName)
    /// Communication bit rate in baud
    Q_PROPERTY(QSerialPort::BaudRate baud READ baud WRITE setBaud)
    /// The parity to be used
    Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity)
    /// Number of data bits
    Q_PROPERTY(QSerialPort::DataBits databits READ databits WRITE setDatabits)
    /// Number of stop bits
    Q_PROPERTY(QSerialPort::StopBits stopbits READ stopbits WRITE setStopbits)
    /// Type of flow control
    Q_PROPERTY(QSerialPort::FlowControl handshake READ handshake WRITE setHandshake)

protected:
    QSerialPort* port_;

public:
    Q_INVOKABLE
    /**
     * @brief QDaqSerial constructor.
     * @param name QDaqObject name
     * @param portName OS name for the serial port, eg, 'COM1' in windows, '/dev/ttyS0' in linux
     */
    explicit QDaqSerial(const QString& name, const QString& portName = QString());

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
/**
 * @brief A class for accesing a MODBUS/TCP connection.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * Used to access MODBUS devices with the TCP/IP protocol.
 * A separate QDaqModbusTcp object must be used per device.
 *
 * Port number is typically 502.
 *
 * QDaqModbusTcp can read/write only MODBUS registers, i.e. 16 bit words.
 *
 * Implementation of MODBUS in QDaq is based on libmodbus.
 *
 */
class QDAQ_EXPORT QDaqModbusTcp : public QDaqTcpip
{
    Q_OBJECT

protected:
    void* ctx_;

public:     
    /**
     * @brief QDaqModbusTcp constructor
     * @param name The QDaqObject name
     * @param ahost Host address as string, eg, '192.168.1.2'
     * @param portno port number, typically 502
     */
    Q_INVOKABLE explicit QDaqModbusTcp(const QString& name, const QString& ahost = QString(), uint portno = 502);
    virtual ~QDaqModbusTcp();

    // io
    virtual int read(uint port, char* buff, int len, int eos = 0);
    virtual int write(uint port, const char* buff, int len, int eos = 0);

protected:
    virtual bool open_();
    virtual void close_();
};
/**
 * @brief A class for accesing a MODBUS/RTU connection.
 *
 * @ingroup Daq
 * @ingroup ScriptAPI
 *
 * Used to access MODBUS devices connected to a serial port.
 * A separate QDaqModbusRtu object must be used per device.
 *
 * Implementation of MODBUS in QDaq is based on libmodbus.
 *
 */
class QDAQ_EXPORT QDaqModbusRtu : public QDaqSerial
{
    Q_OBJECT

protected:
    void* ctx_;

public:     
    /**
     * @brief QDaqModbusRtu constructor
     * @param name QDaqObject name
     * @param portName OS name for the serial port, eg, 'COM1' in windows, '/dev/ttyS0' in linux.
     */
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


