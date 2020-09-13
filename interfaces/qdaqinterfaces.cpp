#include "qdaqinterfaces.h"

#include "QDaqRoot.h"
#include "QDaqSession.h"

#include "core_script_interface.h"
#include "qdaqmodbus.h"
#include "qdaqserial.h"
#include "qdaqtcpip.h"
#include "linuxgpib.h"

QDaqInterfaces* QDaqInterfaces::interfaces_;

QDaqInterfaces::QDaqInterfaces(QObject *parent) : QObject(parent)
{
    interfaces_ = this;

    initScriptInterface( QDaqObject::root()->rootSession() );

    connect(QDaqObject::root(),SIGNAL(newSession(QDaqSession*)),
            this,SLOT(onNewSession(QDaqSession*)));
}

void QDaqInterfaces::onNewSession(QDaqSession *s)
{
    initScriptInterface(s);
}

void QDaqInterfaces::initScriptInterface(QDaqSession *s)
{
    QScriptEngine* e = s->getEngine();
    core_script_register_class(e, &QDaqModbusTcp::staticMetaObject);
    core_script_register_class(e, &QDaqModbusRtu::staticMetaObject);
    core_script_register_class(e, &QDaqSerial::staticMetaObject);
    core_script_register_class(e, &QDaqTcpip::staticMetaObject);
    core_script_register_class(e, &QDaqLinuxGpib::staticMetaObject);
}
