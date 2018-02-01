#include "led_widget.h"
#include "qled.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    QGroupBox* box1 = new QGroupBox("1st LED bar",this);
    QGroupBox* box2 = new QGroupBox("2nd LED bar",this);
    QHBoxLayout* hl1 = new QHBoxLayout();
    QHBoxLayout* hl2 = new QHBoxLayout();
    for(int i=0; i<6; i++)
    {
        ledbar1[i] = new QLed(box1);
        ledbar1[i]->setEnabled(false);
        ledbar1[i]->setChecked(i % 2);
        ledbar1[i]->setColor((QLed::ledColor)((i/2) % 3));

        hl1->addWidget(ledbar1[i]);
        ledbar2[i] = new QLed(box2);
        ledbar2[i]->setReadOnly(false);
        ledbar2[i]->setColor((QLed::ledColor)((i/2) % 3));
        hl2->addWidget(ledbar2[i]);
    }

    box1->setLayout(hl1);
    box2->setLayout(hl2);
    QVBoxLayout* vl = new QVBoxLayout;
    vl->addWidget(box1);
    vl->addWidget(box2);

    setLayout(vl);




}

Widget::~Widget()
{

}
