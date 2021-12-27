#ifndef QDAQFILTERS_H
#define QDAQFILTERS_H

#include "QDaqGlobal.h"

#include <QObject>

class QDaqSession;
class QDaqScriptEngine;

class QDAQ_EXPORT QDaqFilters : public QObject
{
    Q_OBJECT

public:
    explicit QDaqFilters(QObject *parent = 0);

    static void initScriptInterface(QDaqScriptEngine* s);
    static void registerMetaTypes();

private slots:
    void onNewSession(QDaqSession* s);

private:
    static QDaqFilters* filters_;
};

#endif // QDAQFILTERS_H
