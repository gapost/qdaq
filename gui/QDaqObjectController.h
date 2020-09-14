#ifndef RTOBJECTCONTROLLER_H
#define RTOBJECTCONTROLLER_H

#include <QWidget>
#include "QDaqGlobal.h"

class QDaqObjectControllerPrivate;
class QtProperty;
class QDaqObject;

class QDAQ_EXPORT QDaqObjectController : public QWidget
{
    Q_OBJECT
public:
    QDaqObjectController(QWidget *parent = 0);
    ~QDaqObjectController();

    void setObject(QDaqObject *object);
    QDaqObject *object() const;

public slots:
    void updateProperties();
    void updateDynamicProperties();

private:
    QDaqObjectControllerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QDaqObjectController)
    Q_DISABLE_COPY(QDaqObjectController)

private slots:
    void valueChanged(QtProperty*, const QVariant&);
};



#endif
