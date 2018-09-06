#include "qdaqserial.h"

QDaqSerial::QDaqSerial(const QString &name, const QString &portName) :
    QDaqInterface(name)
{
    port_ = new QSerialPort(portName,this); // QextSerialPort::Polling,

    ports_.push_back((QDaqDevice*)0); // only 1 device

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
    QMutexLocker L(&comm_lock);

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
    QMutexLocker L(&comm_lock);

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
    QMutexLocker L(&comm_lock);

    if (port_->open(QIODevice::ReadWrite))
        QDaqInterface::open_();
    else pushError(QString("Open %1 failed").arg(portName()));
    emit propertiesChanged();
    return isOpen();
}

void QDaqSerial::close_()
{
    QMutexLocker L(&comm_lock);
    QDaqInterface::close_();
    port_->close();
}

void QDaqSerial::clear_()
{
    QMutexLocker L(&comm_lock);
    if (port_->isOpen()) port_->flush();
}
