#ifndef QDAQINTERFACES_H
#define QDAQINTERFACES_H

#include "QDaqGlobal.h"

#include <QObject>

class QDaqSession;
class QDaqScriptEngine;

class QDAQ_EXPORT QDaqInterfaces : public QObject
{
    Q_OBJECT
public:
    explicit QDaqInterfaces(QObject *parent = 0);

    static void initScriptInterface(QDaqScriptEngine* s);
    static void registerMetaTypes();

private slots:
    void onNewSession(QDaqSession* s);

private:
    static QDaqInterfaces* interfaces_;
};

#endif // QDAQINTERFACES_H
