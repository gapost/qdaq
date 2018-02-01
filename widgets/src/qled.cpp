#include <QPainter>

#include "qled.h"

const qreal QLed::scaledSize = 1000; /* Visual Studio static const mess */

QLed::QLed(QWidget *parent) : QAbstractButton(parent)
{
    setMinimumSize(24,24);
    setCheckable(true);
    setColor(Green);
    readOnly_ = false;
    setReadOnly(true);
}

void QLed::setColor(ledColor c)
{
    switch (c)
    {
    case Red:
        onColor1 =  QColor(255,0,0);
        onColor2 =  QColor(255,0,0);
        offColor1 = QColor(28,0,0);
        offColor2 = QColor(128,0,0);
        break;
    case Blue:
        onColor1 =  QColor(0,0,255);
        onColor2 =  QColor(0,0,128);
        offColor1 = QColor(0,0,28);
        offColor2 = QColor(0,0,128);
        break;
    case Green:
    default:
        onColor1 =  QColor(0,255,0);
        onColor2 =  QColor(0,128,0); // 192
        offColor1 = QColor(0,28,0);
        offColor2 = QColor(0,128,0);
    }
    update();
}

void QLed::setReadOnly(bool b)
{
    if (b!=readOnly_)
    {
        readOnly_ = b;
        setAttribute(Qt::WA_TransparentForMouseEvents, readOnly_);
        setFocusPolicy(readOnly_ ? Qt::NoFocus : Qt::StrongFocus);
    }
}

void QLed::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    update();
}

void QLed::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    qreal realSize = qMin(width(), height());

    QRadialGradient gradient;
    QPainter painter(this);
    QPen     pen(Qt::black);
             pen.setWidth(1);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width()/2, height()/2);
    painter.scale(realSize/scaledSize, realSize/scaledSize);

    gradient = QRadialGradient (QPointF(-500,-500), 1500, QPointF(-500,-500));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(0,0), 500, 500);

    gradient = QRadialGradient (QPointF(500,500), 1500, QPointF(500,500));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(0,0), 450, 450);

    painter.setPen(pen);
    if( isChecked() ) {
        gradient = QRadialGradient (QPointF(-500,-500), 1500, QPointF(-500,-500));
        gradient.setColorAt(0, onColor1);
        gradient.setColorAt(1, onColor2);
    } else {
        gradient = QRadialGradient (QPointF(500,500), 1500, QPointF(500,500));
        gradient.setColorAt(0, offColor1);
        gradient.setColorAt(1, offColor2);
    }
    painter.setBrush(gradient);
    painter.drawEllipse(QPointF(0,0), 400, 400);
}
