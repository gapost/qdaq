#include "qdaqinterfaces.h"

#include "QDaqRoot.h"
#include "QDaqSession.h"

#include "QDaqScriptAPI.h"
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
    QDaqScriptAPI::registerClass(e, &QDaqModbusTcp::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqModbusRtu::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqSerial::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqTcpip::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqLinuxGpib::staticMetaObject);
}
