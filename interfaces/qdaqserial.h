#ifndef QDAQSERIAL_H
#define QDAQSERIAL_H

#include "QDaqGlobal.h"

#include <QSerialPort>

#include "QDaqInterface.h"

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

#endif // QDAQSERIAL_H
