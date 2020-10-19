#ifndef QDAQERRORLOG_H
#define QDAQERRORLOG_H

#include "QDaqObject.h"

#include <QWidget>

class QTableWidget;
class QPushButton;

class QDAQ_EXPORT QDaqErrorLog : public QWidget
{
    Q_OBJECT
public:
    QDaqErrorLog(QWidget* p = 0);
    virtual ~QDaqErrorLog(void);

public slots:
    void onError(const QDaqError& err);

protected slots:
    void clear();

protected:
    QTableWidget* tableWidget;
    QPushButton* clearButton;
};

#endif // RTERRORLOG_H
