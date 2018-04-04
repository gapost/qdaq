#ifndef NIGPIB_H
#define NIGPIB_H

#include "QDaqGpibPlugin.h"

#include <QObject>
#include <QtPlugin>
#include <QVector>

class QDaqNiGpibPlugin :
        public QObject,
        public QDaqGpibPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqGpibPlugin_iid FILE "ni-gpib.json")
    Q_INTERFACES(QDaqGpibPlugin)

public:
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

    virtual const char* errorMsg(int error_code);
};

#endif // NIGPIB_H
