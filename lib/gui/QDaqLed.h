#ifndef QLED_H
#define QLED_H

#include "QDaqGlobal.h"

#include "3rdparty/kled/kled.h"

class QDAQ_EXPORT QDaqLed : public KLed
{
    Q_OBJECT

    public:
        QDaqLed(QWidget *parent = 0);

public slots:
    void setState(bool f);
    bool isOn() const { return state()==KLed::On; }

};

#endif // QLED_H
