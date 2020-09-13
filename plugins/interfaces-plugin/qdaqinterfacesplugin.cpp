#include "qdaqinterfacesplugin.h"

#include "core_script_interface.h"
#include "qdaqmodbus.h"
#include "qdaqserial.h"
#include "qdaqtcpip.h"
#include "linuxgpib.h"



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
        // init
        core_script_register_class(e, &QDaqModbusTcp::staticMetaObject);
        core_script_register_class(e, &QDaqModbusRtu::staticMetaObject);
        core_script_register_class(e, &QDaqSerial::staticMetaObject);
        core_script_register_class(e, &QDaqTcpip::staticMetaObject);
        core_script_register_class(e, &QDaqLinuxGpib::staticMetaObject);
    }
    else {

         return;
     }
}
