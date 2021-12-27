#include "qdaqinterfaces.h"

#include "QDaqRoot.h"
#include "QDaqSession.h"

#include <QDebug>

#include "QDaqScriptAPI.h"
#include "qdaqmodbus.h"
#include "qdaqserial.h"
#include "qdaqtcpip.h"

#ifdef LINUX_GPIB
    #include "linuxgpib.h"
#endif

QDaqInterfaces* QDaqInterfaces::interfaces_;

QDaqInterfaces::QDaqInterfaces(QObject *parent) : QObject(parent)
{
    interfaces_ = this;
    registerMetaTypes();
    initScriptInterface( QDaqObject::root()->rootSession()->daqEngine() );

    connect(QDaqObject::root(),SIGNAL(newSession(QDaqSession*)),
            this,SLOT(onNewSession(QDaqSession*)));
}

void QDaqInterfaces::onNewSession(QDaqSession *s)
{
    initScriptInterface(s->daqEngine());
}

void QDaqInterfaces::initScriptInterface(QDaqScriptEngine *s)
{
    if (s->type() != QDaqScriptEngine::RootEngine)
    {
        qDebug() << "Cannot install QDaqFilters interface/constructors in non-root QDaqScriptEngine";
        return;
    }
    QScriptEngine* e = s->getEngine();
    QDaqScriptAPI::registerClass(e, &QDaqModbusTcp::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqModbusRtu::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqSerial::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqTcpip::staticMetaObject);

#ifdef LINUX_GPIB
    QDaqScriptAPI::registerClass(e, &QDaqLinuxGpib::staticMetaObject);
#endif

}

void QDaqInterfaces::registerMetaTypes()
{
    qRegisterMetaType<QDaqModbusTcp*>();
    qRegisterMetaType<QDaqModbusRtu*>();
    qRegisterMetaType<QDaqSerial*>();
    qRegisterMetaType<QDaqTcpip*>();

#ifdef LINUX_GPIB
    qRegisterMetaType<QDaqLinuxGpib*>();
#endif

}
