#ifndef QLED_H
#define QLED_H

#include "QDaqGlobal.h"

#include <QAbstractButton>

class QDAQ_EXPORT QDaqLed : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(ledColor color READ color WRITE setColor)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)

    public:
        QDaqLed(QWidget *parent = 0);

        enum ledColor { Red=0, Green, Blue }; //Yellow,Grey,Orange,Purple,Blue };
        Q_ENUM(ledColor)

        ledColor color() const { return color_; }
        void setColor(ledColor c);

        bool readOnly() const { return readOnly_; }
        void setReadOnly(bool b);

    protected:
        virtual void paintEvent (QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);

    private:
        static const qreal scaledSize;  /* init in cpp */
        ledColor color_;
        bool readOnly_;
        QColor  onColor1, offColor1;
        QColor  onColor2, offColor2;
        //QPixmap ledBuffer;
};

#endif // QLED_H
