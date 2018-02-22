#include "linuxgpib.h"

#include <gpib/ib.h>

int QDaqLinuxGpibPlugin::status()
{
    return ThreadIbsta();
}

int QDaqLinuxGpibPlugin::error()
{
    return ThreadIberr();
}
int QDaqLinuxGpibPlugin::count()
{
    return ThreadIbcnt();
}

bool QDaqLinuxGpibPlugin::hasError()
{
    return status() & ERR;
}

void QDaqLinuxGpibPlugin::setTimeout(int boardID, int ms)
{
    static const int TimeoutValues[] = { // in ms
        0, // inf (disabled)
        0, 0, 0, 0, // less than ms
        1, 3,
        10, 30,
        100, 300,
        1000,    3000,
        10000,   30000,
        100000,  300000, 1000000
    };
    int c; //(timeout code)
    if (ms==0) c = TNONE;
    else
    {
        c = 5;
        while(c<17 && ms>TimeoutValues[c]) c++;
    }
    ibconfig(boardID,IbcTMO,c);
}

void QDaqLinuxGpibPlugin::SendIFC(int boardID)
{
    ::SendIFC(boardID);
}
void QDaqLinuxGpibPlugin::EnableRemote(int boardID, int id)
{
    Addr4882_t addr[2] = {(Addr4882_t)id, NOADDR};
    ::EnableRemote(boardID,addr);
}
void QDaqLinuxGpibPlugin::EnableLocal(int boardID, int id)
{
    Addr4882_t addr[2] = {(Addr4882_t)id, NOADDR};
    ::EnableLocal(boardID,addr);
}
void QDaqLinuxGpibPlugin::QDaqLinuxGpibPlugin::DeviceClear(int boardID, int address)
{
    ::DevClear(boardID,(Addr4882_t)address);
}
void QDaqLinuxGpibPlugin::Receive(int boardID, int address, char* data, int len, int eoi)
{
    ::Receive(boardID,(Addr4882_t)address, data, len, eoi);
}
void QDaqLinuxGpibPlugin::Send(int boardID, int address, const char* data, int len, int eotmode)
{
    ::Send(boardID,(Addr4882_t)address, data, len, eotmode);
}
int QDaqLinuxGpibPlugin::ReadStatusByte(int boardID, int address)
{
    short ret(0);
    ::ReadStatusByte(boardID,(Addr4882_t)address, &ret);
    return ret;
}
void QDaqLinuxGpibPlugin::FindListeners(int boardID, const QVector<int>& addresses, QVector<int>& results)
{
    Addr4882_t addrlist[32], results_[32];
    int limit = 31;
    int n =  addresses.size() > 31 ? 31 : addresses.size();
    for(int i=0; i<n; i++) addrlist[i] = addresses[i];
    addrlist[n] = NOADDR;

    ::FindLstn(boardID, addrlist, results_, limit);

    if (!(status() & ERR))
    {
        results.resize(count());
        for(int i=0; i<count(); ++i) results[i] = results_[i];
    }
}
const char* QDaqLinuxGpibPlugin::errorMsg(int error_code)
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
    const int N_ERROR_CODES = 21;

    if (error_code>=0 && error_code<N_ERROR_CODES) return message[error_code];
    else return message[N_ERROR_CODES];
}
