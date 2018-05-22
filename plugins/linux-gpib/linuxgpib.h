#ifndef LINUXGPIB_H
#define LINUXGPIB_H

#include "linux-gpib_global.h"

#include "QDaqGpibPlugin.h"

//#include <QObject>
#include "QDaqObject.h"
#include <QtPlugin>
#include <QVector>

class LINUXGPIBSHARED_EXPORT QDaqLinuxGpibPlugin :
        public QDaqObject,
        public QDaqGpibPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqGpibPlugin_iid FILE "qdaqlinuxgpib.json")
    Q_INTERFACES(QDaqGpibPlugin)

public:  
    QDaqLinuxGpibPlugin() : QDaqObject("linux-gpib")
    {}
    virtual int status();
    virtual int error();
    virtual int count();
    virtual bool hasError();

    virtual void setTimeout(int boardID, int ms);

    virtual void SendIFC(int boardID);
    virtual void EnableRemote(int boardID, int id);
    virtual void EnableLocal(int boardID, int id);
    virtual void DeviceClear(int boardID, int address);
    virtual void Receive(int boardID, int address, char* data, int len, int eoi);
    virtual void Send(int boardID, int address, const char* data, int len, int eotmode);
    virtual int ReadStatusByte(int boardID, int address);
    virtual void FindListeners(int boardID, const QVector<int>& addresses, QVector<int>& results);
    virtual void Trigger(int boardID, int address);
    virtual void TriggerList(int boardID, const QVector<int>& addresses);

    virtual const char* errorMsg(int error_code);
};

#endif // LINUXGPIB_H
