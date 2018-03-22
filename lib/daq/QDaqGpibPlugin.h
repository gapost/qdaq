#ifndef QDAQGPIBPLUGIN_H
#define QDAQGPIBPLUGIN_H

#include <QtPlugin>

/**
 * @brief Interface definition for QDaq GPIB plugins.
 *
 * @ingroup Daq
 *
 * A GPIB plugin is created by sub-classing QDaqGpibPlugin
 * and implementing all its virtual functions.
 *
 * The interface defines a number of GPIB specific functions
 * that are needed in order to operate a GPIB interface according
 * to the QDaqInterface class.
 *
 */
class QDaqGpibPlugin
{
public:
    virtual ~QDaqGpibPlugin() {}

    virtual int status() = 0;
    virtual int error() = 0;
    virtual int count() = 0;
    virtual bool hasError() = 0;

    virtual void setTimeout(int boardID, int ms) = 0;

    virtual void SendIFC(int boardID) = 0;
    virtual void EnableRemote(int boardID, int address) = 0;
    virtual void EnableLocal(int boardID, int address) = 0;
    virtual void DeviceClear(int boardID, int address) = 0;
    virtual void Receive(int boardID, int address, char* data, int len, int eoi) = 0;
    virtual void Send(int boardID, int address, const char* data, int len, int eotmode) = 0;
    virtual int ReadStatusByte(int boardID, int address) = 0;
    virtual void FindListeners(int boardID, const QVector<int>& addresses, QVector<int>& results) = 0;

    virtual const char* errorMsg(int error_code) = 0;
};

/// An identifier to be used in IID metadata of GPIB plugins
#define QDaqGpibPlugin_iid "org.qdaq.gpibplugin"

Q_DECLARE_INTERFACE(QDaqGpibPlugin, QDaqGpibPlugin_iid)

#endif // QDAQGPIBPLUGIN_H
