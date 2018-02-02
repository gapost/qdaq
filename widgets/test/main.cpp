#include "led_widget.h"
#include <QApplication>

#include "QDaqPlotWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    QDaqPlotWidget P;
    int n = 100;
    QDaqBuffer x(n+1), y(n+1);
    for(int i=0; i<=n; i++)
    {
        x << 2.*i/n - 1.;
        y << x[i]*x[i]*x[i];
    }
    P.resize(600,400);
    P.show();
    P.plot(x,y);
    P.setTitle("y = x^3");
    P.setXLabel("x");
    P.setYLabel("y");

    return a.exec();
}
