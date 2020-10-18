#include "QDaqPlotWidget.h"

#include <math.h>


QDaqPlotWidget::QDaqPlotWidget(QWidget* parent) :
    QMatPlotWidget(parent)
{
}

QDaqPlotWidget::~QDaqPlotWidget(void)
{
}

class QDaqPlotDataSeries : public QMatPlotWidget::AbstractDataSeries
{
    QDaqVector vx;
    QDaqVector vy;
public:
    QDaqPlotDataSeries(const QDaqVector& x, const QDaqVector& y) : vx(x), vy(y)
    {
    }
    QDaqPlotDataSeries(const QDaqPlotDataSeries& other) : vx(other.vx), vy(other.vy)
    {
    }
    virtual ~QDaqPlotDataSeries()
    {
    }
    int size() const override { return qMin(vx.size(),vy.size()); }
    QPointF sample( int i ) const override { return QPointF(vx[i],vy[i]); }
    QRectF boundingRect() const override
    {
        double x1 = vx.vmin(), x2 = vx.vmax();
        double y1 = vy.vmin(), y2 = vy.vmax();
        return QRectF(x1,y1,x2-x1,y2-y1);
    }
};

void QDaqPlotWidget::plot(const QDaqVector &x, const QDaqVector &y, const QString &attr, const QColor &clr)
{
    QDaqPlotDataSeries* data = new QDaqPlotDataSeries(x,y);

    QMatPlotWidget::plotDataSeries(data,attr,clr);

}

void QDaqPlotWidget::plot(const QDaqVector &x, const QDaqVector &y, const QColor &clr)
{
    plot(x,y,"",clr);

}


