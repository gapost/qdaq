#include "nigpib.h"

#include <windows.h>
#include <decl-32.h>

PluginFactory::PluginFactory() : QObject()
{
}

QList<const QMetaObject *> PluginFactory::pluginClasses() const
{
    QList<const QMetaObject *> lst;
    lst << &QDaqNiGpib::staticMetaObject;
    return lst;
}

/***********/

QDaqNiGpib::QDaqNiGpib(const QString& name) :
QDaqInterface(name)
{
    // 32 empty places
    ports_.fill(0, 32);
}

QDaqNiGpib::~QDaqNiGpib()
{
}

void QDaqNiGpib::pushGpibError(int code, const QString& comm)
{
    static const char* message[] = {
        "EDVR: System error",
        "ECIC: Function requires GPIB interface to be CIC",
        "ENOL: No Listeners on the GPIB",
        "EADR: GPIB interface not addressed correctly",
        "EARG: Invalid argument to function call",
        "ESAC: GPIB interface not System Controller as required",
        "EABO: I/O operation aborted (timeout)",
        "ENEB: Nonexistent GPIB interface",
        "EDMA: DMA error",
        "Unknown Error",
        "EOIP: Asynchronous I/O in progress",
        "ECAP: No capability for operation",
        "EFSO: File system error",
        "Unknown Error",
        "EBUS: GPIB bus error",
        "ESTB: Serial poll status byte queue overflow",
        "ESRQ: SRQ stuck in ON position",
        "Unknown Error",
        "Unknown Error",
        "Unknown Error",
        "ETAB: Table problem",
        "Unknown Error"
    };
    const int N_ERROR_CODES = 22;

    int error_code = (code<0 || code>=N_ERROR_CODES) ? N_ERROR_CODES - 1 : code;
    pushError(message[error_code],comm);

}

bool QDaqNiGpib::open_()
{
    if (isOpen()) return true;

    QMutexLocker L(&comm_lock);

    ::SendIFC(address());

    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("SendIFC(%1)").arg(address())
            );
    }
    else QDaqInterface::open_();

    if (isOpen())
    {
        setTimeout_(timeout());
    }

    emit propertiesChanged();

    return isOpen();
}

bool QDaqNiGpib::open_port(uint id, QDaqDevice* dev)
{
    QMutexLocker L(&comm_lock);
    if (QDaqInterface::open_port(id,dev))
    {
        Addr4882_t addr[2] = {(Addr4882_t)id, NOADDR};
        ::EnableRemote(address(),addr);
        if (ThreadIbsta() & ERR)
        {
            pushGpibError(
                ThreadIberr(),
                QString("EnableRemote(%1,%2)").arg(address()).arg(id)
            );
            QDaqInterface::close_port(id);
            return false;
        }
        else return true;
    }
    else return false;
}

void QDaqNiGpib::close_port(uint id)
{
    QMutexLocker L(&comm_lock);
    Addr4882_t addr[2] = {(Addr4882_t)id, NOADDR};
    ::EnableLocal(address(), addr);
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("EnableLocal(%1,%2)").arg(address()).arg(id)
        );
    }
    QDaqInterface::close_port(id);
}

void QDaqNiGpib::clear_port(uint id)
{
    QMutexLocker L(&comm_lock);
    ::DevClear(address(),(Addr4882_t)id);
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("DeviceClear(%1,%2)").arg(address()).arg(id)
            );
    }
}

int QDaqNiGpib::read(uint port, char* data, int len, int eoi)
{
    QMutexLocker L(&comm_lock);
    ::Receive(address(),(Addr4882_t)port, data, len, eoi);
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("Recieve(%1,%2,%3,%4)").arg(address()).arg(port).arg(len).arg(eoi)
            );
        return 0;
    }
    else return ::ThreadIbcnt();
}

int QDaqNiGpib::readStatusByte(uint port)
{
    QMutexLocker L(&comm_lock);
    short ret(0);
    ::ReadStatusByte(address(),(Addr4882_t)port, &ret);

    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("ReadStatusByte(%1,%2)").arg(address()).arg(port)
            );
        return 0;
    }
    else return ret;
}

int QDaqNiGpib::write(uint port, const char* buff, int len, int e)
{
    QMutexLocker L(&comm_lock);
    int eot =  (e & 0x0000FF00) >> 8;
    ::Send(address(),(Addr4882_t)port, (void *)buff, len, eot);
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("Send(%1,%2,%3,%4,%5)").arg(address()).arg(port)
                .arg(QString(QByteArray(buff,len))).arg(len).arg(eot)
            );
        return 0;
    }
    else return len; // ibcntl; linux-gpib sends +1 byte
}

void QDaqNiGpib::trigger(uint id)
{
     QMutexLocker L(&comm_lock);
    ::Trigger(address(),(Addr4882_t)id);
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("Trigger(%1,%2)").arg(address()).arg(id)
            );
    }
}

void QDaqNiGpib::clear_()
{
    QMutexLocker L(&comm_lock);
    ::SendIFC(address());
    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("SendIFC(%1)").arg(address())
            );
    }
}

void QDaqNiGpib::setTimeout_(uint ms)
{
    static const uint TimeoutValues[] = { // in ms
        0, // inf (disabled)
        0, 0, 0, 0, // less than ms
        1, 3,
        10, 30,
        100, 300,
        1000,    3000,
        10000,   30000,
        100000,  300000, 1000000
    };


    QMutexLocker L(&comm_lock);

    int c; //(timeout code)
    if (ms==0) c = TNONE;
    else
    {
        c = 5;
        while(c<17 && ms>TimeoutValues[c]) c++;
    }
    ::ibconfig(address(),IbcTMO,c);


    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("setTimeout(%1,%2)").arg(address()).arg(ms)
            );
    }
}

QDaqIntVector QDaqNiGpib::findListeners()
{
    QMutexLocker L(&comm_lock);

    QDaqIntVector Listeners;
    Addr4882_t addrlist[31], results_[30];

    for(int i=0; i<30; i++) addrlist[i] = i+1;
    addrlist[30] = NOADDR;

    ::FindLstn(address(), addrlist, results_, 31);

    if (ThreadIbsta() & ERR)
    {
        pushGpibError(
            ThreadIberr(),
            QString("FindListeners(%1,array,array)").arg(address())
            );
    }
    else
    {
        int n = ThreadIbcnt();
        Listeners.resize(n);
        for(int i=0; i<n; ++i) Listeners[i] = results_[i];
    }

    return Listeners;
}





