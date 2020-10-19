#include "QDaqLed.h"

QDaqLed::QDaqLed(QWidget *parent) : KLed(Qt::red, parent)
{
}


void QDaqLed::setState(bool f)
{
    if (f) on(); else off();
}
