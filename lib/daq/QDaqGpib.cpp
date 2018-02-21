#include "QDaqGpib.h"

// NI defs
    enum StatusCode {
        Dcas = 0x0001, // brd Device Clear State
        Dtas = 0x0002, // brd Device Trigger State
        Lacs = 0x0004, // brd Listener
        Tacs = 0x0008, // brd Talker
        Atn  = 0x0010, // brd Attention is asserted
        Cic  = 0x0020, // brd Controller-In-Charge
        Rem  = 0x0040, // brd Remote State
        Lok  = 0x0080, // brd Lockout State
        Cmpl = 0x0100, // dev, brd I/O completed
        Rqs  = 0x0800, // dev Device requesting service
        Srqi = 0x1000, // brd SRQ interrupt received
        End  = 0x2000, // dev, brd END or EOS detected
        Timo = 0x4000, // dev, brd Time limit exceeded
        Err  = 0x8000  // dev, brd NI-488.2 error
    };

    enum ErrorCode {
        Edvr = 0 , // 0  EDVR System error
        Ecic = 1 , // 1  ECIC Function requires GPIB interface to be CIC
        Enol = 2 , // 2  ENOL No Listeners on the GPIB
        Eadr = 3 , // 3  EADR GPIB interface not addressed correctly
        Earg = 4 , // 4  EARG Invalid argument to function call
        Esac = 5 , // 5  ESAC GPIB interface not System Controller as required
        Eabo = 6 , // 6  EABO I/O operation aborted (timeout)
        Eneb = 7 , // 7  ENEB Nonexistent GPIB interface
        Edma = 8 , // 8  EDMA DMA error
        Eoip = 10, // 10 EOIP Asynchronous I/O in progress
        Ecap = 11, // 11 ECAP No capability for operation
        Efso = 12, // 12 EFSO File system error
        Ebus = 14, // 14 EBUS GPIB bus error
        Estb = 15, // 15 ESTB Serial poll status byte queue overflow
        Esrq = 16, // 16 ESRQ SRQ stuck in ON position
        Etab = 20  // 20 ETAB Table problem
    };

    enum TimeoutCode
    {
        tNONE   = 0,   /* Infinite timeout (disabled)        */
        t10us   = 1,   /* Timeout of 10 us (ideal)           */
        t30us   = 2,   /* Timeout of 30 us (ideal)           */
        t100us  = 3,   /* Timeout of 100 us (ideal)          */
        t300us  = 4,   /* Timeout of 300 us (ideal)          */
        t1ms    = 5,   /* Timeout of 1 ms (ideal)            */
        t3ms    = 6,   /* Timeout of 3 ms (ideal)            */
        t10ms   = 7,   /* Timeout of 10 ms (ideal)           */
        t30ms   = 8,   /* Timeout of 30 ms (ideal)           */
        t100ms  = 9,   /* Timeout of 100 ms (ideal)          */
        t300ms  =10,   /* Timeout of 300 ms (ideal)          */
        t1s     =11,   /* Timeout of 1 s (ideal)             */
        t3s     =12,   /* Timeout of 3 s (ideal)             */
        t10s    =13,   /* Timeout of 10 s (ideal)            */
        t30s    =14,   /* Timeout of 30 s (ideal)            */
        t100s   =15,   /* Timeout of 100 s (ideal)           */
        t300s   =16,   /* Timeout of 300 s (ideal)           */
        t1000s  =17    /* Timeout of 1000 s (ideal)          */
    };

QDaqGpib::QDaqGpib(const QString& name) :
QDaqInterface(name), gpib_(0)
{
    // 32 empty places
    ports_.fill(0, 32);
}
QDaqGpib::~QDaqGpib()
{
}

void QDaqGpib::pushGpibError(int code, const QString& comm)
{
    pushError(errorCode(code),comm);
}

const char* QDaqGpib::errorCode(int idx)
{
    static const char* code[] = {
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
    const int N_ERROR_CODES = 21;

    if (idx>=0 && idx<N_ERROR_CODES) return code[idx];
    else return code[N_ERROR_CODES];
}

bool QDaqGpib::open_()
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

    if (isOpen()) return true;

    os::auto_lock L(comm_lock);

    gpib_->SendIFC(addr_);

    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("SendIFC(%1)").arg(addr_)
            );
    }
    else QDaqInterface::open_();

    if (isOpen())
    {
        gpib_->setTimeout(addr_, this->timeout_);
    }

    emit propertiesChanged();

    return isOpen();
}

bool QDaqGpib::open_port(uint id, QDaqDevice* dev)
{
    os::auto_lock L(comm_lock);
    if (QDaqInterface::open_port(id,dev))
    {
        gpib_->EnableRemote(addr_, id);
        if (gpib_->status() & Err)
        {
            pushGpibError(
                gpib_->error(),
                QString("EnableRemote(%1,%2)").arg(addr_).arg(id)
            );
            QDaqInterface::close_port(id);
            return false;
        }
        else return true;
    }
    else return false;
}

void QDaqGpib::close_port(uint id)
{
    os::auto_lock L(comm_lock);
    gpib_->EnableLocal(addr_, id);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("EnableLocal(%1,%2)").arg(addr_).arg(id)
        );
    }
    QDaqInterface::close_port(id);
}

void QDaqGpib::clear_port(uint id)
{
    os::auto_lock L(comm_lock);
    gpib_->DeviceClear(addr_,id);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("DeviceClear(%1,%2)").arg(addr_).arg(id)
            );
    }
}

int QDaqGpib::read(uint port, char* data, int len, int eoi)
{
    os::auto_lock L(comm_lock);
    gpib_->Receive(addr_, port, data, len, eoi);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("Recieve(%1,%2,%3,%4)").arg(addr_).arg(port).arg(len).arg(eoi)
            );
        return 0;
    }
    else return gpib_->count();
}

int QDaqGpib::readStatusByte(uint port)
{
    os::auto_lock L(comm_lock);
    int result = gpib_->ReadStatusByte (addr_, port);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("ReadStatusByte(%1,%2)").arg(addr_).arg(port)
            );
        return 0;
    }
    else return result;
}

int QDaqGpib::write(uint port, const char* buff, int len, int e)
{
    os::auto_lock L(comm_lock);
    int eot =  (e & 0x0000FF00) >> 8;
    gpib_->Send(addr_, port, buff, len, eot);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("Send(%1,%2,%3,%4,%5)").arg(addr_).arg(port)
                .arg(QString(QByteArray(buff,len))).arg(len).arg(eot)
            );
        return 0;
    }
    else return len; // ibcntl; linux-gpib sends +1 byte
}

void QDaqGpib::clear_()
{
    os::auto_lock L(comm_lock);
    gpib_->SendIFC(addr_);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("SendIFC(%1)").arg(addr_)
            );
    }
}

QDaqIntVector QDaqGpib::findListeners()
{
    os::auto_lock L(comm_lock);

    QDaqIntVector Addresses(31), Listeners;

    for(int i=1; i<32; i++) Addresses[i-1] = i;

    gpib_->FindListeners(addr_,Addresses,Listeners);
    if (gpib_->status() & Err)
    {
        pushGpibError(
            gpib_->error(),
            QString("FindListeners(%1,array,array)").arg(addr_)
            );
    }
    return Listeners;
}


