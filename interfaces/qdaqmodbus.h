#ifndef QDAQMODBUS_H
#define QDAQMODBUS_H

#include "qdaqserial.h"
#include "qdaqtcpip.h"

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

#endif // QDAQMODBUS_H
