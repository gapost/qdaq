#include "qdaqmodbus.h"

#include <modbus/modbus.h>

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

    QMutexLocker L(&comm_lock);

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
        QDaqInterface::close_();
    }
    else {
        QDaqInterface::open_();
        ctx_ = ctx;
        modbus_set_slave(ctx,1);
    }

    emit propertiesChanged();

    return isOpen();
}

void QDaqModbusRtu::close_()
{
    QMutexLocker L(&comm_lock);
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
    QMutexLocker L(&comm_lock);

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
    QMutexLocker L(&comm_lock);

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

/********************** QDaqModbusTcp ******************************/

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

    QMutexLocker L(&comm_lock);

    /* TCP */
    modbus_t* ctx = modbus_new_tcp(host().toLatin1().constData(), port_);

#if LIBMODBUS_VERSION_MINOR > 0
    modbus_set_response_timeout(ctx, 0, timeout()*1000);
#else
    timeval response_timeout;
    /* Define a new and too short timeout! */
    response_timeout.tv_sec = 0;
    response_timeout.tv_usec = timeout()*1000;
    modbus_set_response_timeout(ctx, &response_timeout);
#endif


    if (modbus_connect(ctx) == -1) {
        pushError("modbus_connect failed", modbus_strerror(errno));
        modbus_free(ctx);
        QDaqInterface::close_();
    }
    else {
        QDaqInterface::open_();
        ctx_ = ctx;
    }

    emit propertiesChanged();

    return isOpen();
}

void QDaqModbusTcp::close_()
{
    QMutexLocker L(&comm_lock);
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
    QMutexLocker L(&comm_lock);

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
    QMutexLocker L(&comm_lock);

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
