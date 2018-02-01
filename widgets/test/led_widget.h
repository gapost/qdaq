#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QLed;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    QLed* ledbar1[6];
    QLed* ledbar2[6];

};

#endif // WIDGET_H
