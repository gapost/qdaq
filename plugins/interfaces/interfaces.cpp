#include "interfaces.h"
#include "qdaqserial.h"
#include "qdaqtcpip.h"
#include "qdaqmodbus.h"

Interfaces::Interfaces() : QObject()
{
}

QList<const QMetaObject *> Interfaces::pluginClasses() const
{
    QList<const QMetaObject *> lst;
    lst << &QDaqSerial::staticMetaObject;
    lst << &QDaqTcpip::staticMetaObject;
    lst << &QDaqModbusRtu::staticMetaObject;
    lst << &QDaqModbusTcp::staticMetaObject;
    return lst;
}
