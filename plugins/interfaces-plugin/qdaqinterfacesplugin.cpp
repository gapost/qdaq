#include "qdaqinterfacesplugin.h"

#include <QScriptEngine>

#include "QDaqScriptAPI.h"
#include "QDaqSession.h"
#include "qdaqmodbus.h"
#include "qdaqserial.h"
#include "qdaqtcpip.h"
#include "linuxgpib.h"
#include "qdaqinterfaces.h"

#include <QScriptEngine>

QDaqInterfacesPlugin::QDaqInterfacesPlugin(QObject *parent) :
    QScriptExtensionPlugin(parent)
{
}

QStringList QDaqInterfacesPlugin::keys() const
{
    QStringList lst;
    lst << "qdaq-interfaces";
    lst << "qdaq";
    return lst;
}

void QDaqInterfacesPlugin::initialize(const QString &key, QScriptEngine *e)
{
    if (key=="qdaq") {}
    else if (key=="qdaq-interfaces") {
        // register QDaqInterfaces types with meta-object system
        QDaqInterfaces::registerMetaTypes();
        // If it is the root QDaq script engine, register the constructors
        QDaqScriptEngine* daqEngine = qobject_cast<QDaqScriptEngine*>(e->parent());
        if (daqEngine && daqEngine->type()==QDaqScriptEngine::RootEngine)
        {
            QDaqScriptAPI::registerClass(e, &QDaqModbusTcp::staticMetaObject);
            QDaqScriptAPI::registerClass(e, &QDaqModbusRtu::staticMetaObject);
            QDaqScriptAPI::registerClass(e, &QDaqSerial::staticMetaObject);
            QDaqScriptAPI::registerClass(e, &QDaqTcpip::staticMetaObject);
            QDaqScriptAPI::registerClass(e, &QDaqLinuxGpib::staticMetaObject);
        }
    }
    else {

         return;
     }
}
