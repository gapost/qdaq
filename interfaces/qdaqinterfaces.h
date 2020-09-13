#ifndef QDAQINTERFACES_H
#define QDAQINTERFACES_H

#include "QDaqGlobal.h"

#include <QObject>

class QDaqSession;

class QDAQ_EXPORT QDaqInterfaces : public QObject
{
    Q_OBJECT
public:
    explicit QDaqInterfaces(QObject *parent = 0);

    static void initScriptInterface(QDaqSession* s);

private slots:
    void onNewSession(QDaqSession* s);

private:
    static QDaqInterfaces* interfaces_;
};

#endif // QDAQINTERFACES_H
