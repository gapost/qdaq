#include "QDaqPlotWidget.h"

#include <QDaqBuffer>

#include <QCloseEvent>
#include <QCoreApplication>
#include <QSet>

#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_math.h>

#include <math.h>

class FormattedPicker : public QwtPlotPicker
{
public:
    FormattedPicker(int xAxis, int yAxis,
        RubberBand rubberBand, DisplayMode trackerMode,
        QWidget *pc) : QwtPlotPicker(xAxis,yAxis,rubberBand,trackerMode,pc)
    {
    }
protected:
    virtual QwtText trackerTextF	(	const QPointF & 	pos	 ) 	 const
    {
        QwtText lx = plot()->axisScaleDraw(QwtPlot::xBottom)->label(pos.x());
        QwtText ly = plot()->axisScaleDraw(QwtPlot::yLeft)->label(pos.y());
        QString S( lx.text() );
        S += QChar(',');
        S += ly.text();
        return S;
    }
};

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QWidget *canvas):
        QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        //setMaxStackDepth(1);
    }
protected:
    virtual void begin ()
    {
        //const QwtDoubleRect &rect = scaleRect();
        //const QwtDoubleRect &brect = zoomBase();
        if (zoomRectIndex()==0)
        {
            setZoomBase(false);

            const QRectF &rect = this->scaleRect();
            QwtPlot *plt = plot();
            if (plt->axisAutoScale(QwtPlot::xBottom))
            {
                plt->setAxisScale(QwtPlot::xBottom,rect.left(),rect.right());
            }
            if (plt->axisAutoScale(QwtPlot::yLeft))
            {
                plt->setAxisScale(QwtPlot::yLeft,rect.top(),rect.bottom());
            }
        }

        QwtPlotZoomer::begin();
    }
    virtual bool end (bool ok=true)
    {
        return QwtPlotZoomer::end(ok);
    }
    virtual void widgetMouseDoubleClickEvent (QMouseEvent *)
    {
        QwtPlot *plt = plot();
        if ( !plt ) return;
        plt->setAxisAutoScale(QwtPlot::xBottom);
        plt->setAxisAutoScale(QwtPlot::yLeft);

        setZoomStack(zoomStack(),0); //QStack<QwtDoubleRect>());
    }
};


class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw()
    {
    }
    virtual QwtText label(double v) const
    {
        QDaqTimeValue t(v);
        return QDateTime(t).toString("hh:mm:ss");
    }
};

class SciScaleDraw: public QwtScaleDraw
{
public:
    SciScaleDraw()
    {
    }
    virtual QwtText label(double v) const
    {
        return QString::number(v,'g');
    }
};

class TimeScaleEngine: public QwtLinearScaleEngine
{
protected:
    static double conversion_factor(double T)
    {
        double C = 1.; // conversion const
        if (T<=60) C = 1.; // secs up to 1 min
        else if (T<=3600) C = 60; // mins up to 1hr
        else if (T<=24*3600) C = 60*60; // hr up to 1 day
        else C = 24*60*60; // else in days
        return C;
    }
public:
    virtual void autoScale(int maxSteps,
        double &x1, double &x2, double &stepSize) const
    {
        double C = conversion_factor(fabs(x2-x1));
        x1 /= C; x2 /= C; // convert to  s,m,hr,d ...
        QwtLinearScaleEngine::autoScale(maxSteps,x1,x2,stepSize);
        x1 *= C; x2 *= C; stepSize *= C; // convert to s
    }
};


class QDAQ_EXPORT QDaqPlotData : public QwtSeriesData< QPointF >
{
    QDaqBuffer vx;
    QDaqBuffer vy;
    size_t sz;
public:
    QDaqPlotData(const QDaqBuffer& x, const QDaqBuffer& y) : vx(x), vy(y)
    {
        sz = qMin(vx.size(),vy.size());
    }
    QDaqPlotData(const QDaqPlotData& other) : vx(other.vx), vy(other.vy), sz(other.sz)
    {
    }
    virtual ~QDaqPlotData()
    {
    }

    QDaqPlotData *copy() const
    {
        QDaqPlotData* cc = new QDaqPlotData(*this);
        return cc;
    }

    virtual size_t size() const { return sz; }
    virtual QPointF sample( size_t i ) const { return QPointF(vx[i],vy[i]); }

    double x(size_t i) const { return vx[i]; }
    double y(size_t i) const { return vy[i]; }

    virtual QRectF boundingRect() const
    {
        const_cast<QDaqPlotData*>(this)->sz = qMin(vx.size(),vy.size());
        double x1 = vx.vmin(), x2 = vx.vmax();
        double y1 = vy.vmin(), y2 = vy.vmax();
        return QRectF(x1,y1,x2-x1,y2-y1);
    }

    void update(const QDaqPlotWidget* w, const QDaqBuffer* v)
    {
        Q_UNUSED(v);
        sz = qMin(vx.size(),vy.size());
        //if (v==&vx && w->axisAutoScale(QwtPlot::xBottom)) vx.calcBounds(x1,x2);
        //else if (v==&vy && w->axisAutoScale(QwtPlot::yLeft)) vy.calcBounds(y1,y2);
        //double x1,x2,y1,y2;
        if (w->axisAutoScale(QwtPlot::xBottom)) vx.vmin();
        if (w->axisAutoScale(QwtPlot::yLeft)) vy.vmin();

    }
};

QDaqPlotWidget::QDaqPlotWidget(QWidget* parent) :
    QwtPlot(parent),
    timeScaleX_(false), timeScaleY_(false),
    grid_on_(false),
    id_(0)
{
    setCanvasBackground(QColor(Qt::white));

    QFont font;
    font = axisFont(QwtPlot::xBottom);
    font.setPointSize(8);
    setAxisFont(QwtPlot::xBottom,font);
    setAxisFont(QwtPlot::yLeft,font);

    QwtText txt = QwtPlot::title();
    font = txt.font();
    font.setPointSize(10);
    txt.setFont(font);
    QwtPlot::setTitle(txt);

    txt = axisTitle(QwtPlot::xBottom);
    font = txt.font();
    font.setPointSize(9);
    txt.setFont(font);
    setAxisTitle(QwtPlot::xBottom,txt);
    setAxisTitle(QwtPlot::yLeft,txt);


    grid_ = new QwtPlotGrid;
    grid_->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
    //grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid_->attach(this);
    grid_->enableX(grid_on_);
    grid_->enableY(grid_on_);

    setAxisScaleDraw(QwtPlot::xBottom, new SciScaleDraw());
    setAxisScaleDraw(QwtPlot::yLeft, new SciScaleDraw());

    zoomer = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, canvas());
    zoomer->setRubberBand(QwtPicker::RectRubberBand);
    zoomer->setRubberBandPen(QPen(Qt::darkGray,0,Qt::DashLine));

    //zoomer->setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
    zoomer->setTrackerMode(QwtPicker::AlwaysOff);

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size


    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
            Qt::RightButton);

    panner = new QwtPlotPanner(canvas());
    panner->setMouseButton(Qt::LeftButton,Qt::ShiftModifier);

    picker = new FormattedPicker(QwtPlot::xBottom, QwtPlot::yLeft,
//        QwtPicker::PointSelection | QwtPicker::DragSelection,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        canvas());
    picker->setRubberBand(QwtPicker::CrossRubberBand);

    setAutoReplot(true);

}

QDaqPlotWidget::~QDaqPlotWidget(void)
{
}

//getters
QString QDaqPlotWidget::title() const
{
    return QwtPlot::title().text();
}
QString QDaqPlotWidget::xlabel() const
{
    return axisTitle(QwtPlot::xBottom).text();
}
QString QDaqPlotWidget::ylabel() const
{
    return axisTitle(QwtPlot::yLeft).text();
}
bool QDaqPlotWidget::autoScaleX() const
{
    return axisAutoScale(QwtPlot::xBottom);
}
bool QDaqPlotWidget::autoScaleY() const
{
    return axisAutoScale(QwtPlot::yLeft);
}
QPointF QDaqPlotWidget::xlim() const
{
    return QPointF(axisScaleDiv(QwtPlot::xBottom).lowerBound(),
                       axisScaleDiv(QwtPlot::xBottom).upperBound());
}
QPointF QDaqPlotWidget::ylim() const
{
    return QPointF(axisScaleDiv(QwtPlot::yLeft).lowerBound(),
                       axisScaleDiv(QwtPlot::yLeft).upperBound());
}



//setters
void QDaqPlotWidget::setTitle(const QString& s)
{
   QwtPlot::setTitle(s);
}
void QDaqPlotWidget::setXLabel(const QString& s)
{
    setAxisTitle(QwtPlot::xBottom,s);
}
void QDaqPlotWidget::setYLabel(const QString& s)
{
    setAxisTitle(QwtPlot::yLeft,s);
}
void QDaqPlotWidget::setAutoScaleX(bool on)
{
    if (on) setAxisAutoScale(QwtPlot::xBottom);
}
void QDaqPlotWidget::setAutoScaleY(bool on)
{
    if (on) setAxisAutoScale(QwtPlot::yLeft);
}
void QDaqPlotWidget::setTimeScaleX(bool on)
{
    if (on==timeScaleX_) return;
    setTimeAxis(QwtPlot::xBottom, on);
    timeScaleX_ = on;
}
void QDaqPlotWidget::setTimeScaleY(bool on)
{
    if (on==timeScaleY_) return;
    setTimeAxis(QwtPlot::yLeft, on);
    timeScaleY_ = on;
}
void QDaqPlotWidget::setGrid(bool on)
{
    if (grid_on_==on) return;
    grid_->enableX(on);
    grid_->enableY(on);
    grid_on_ = on;
}
void QDaqPlotWidget::setXlim(const QPointF& v)
{
    setAxisScale(QwtPlot::xBottom,v.x(),v.y()) ;
}
void QDaqPlotWidget::setYlim(const QPointF &v)
{
    setAxisScale(QwtPlot::yLeft,v.x(),v.y()) ;
}

void QDaqPlotWidget::plot(const QDaqBuffer &x, const QDaqBuffer &y)
{
    static const Qt::GlobalColor eight_colors[8] =
    {
        Qt::blue,
        Qt::red,
        Qt::darkGreen,
        Qt::magenta,
        Qt::darkBlue,
        Qt::darkMagenta,
        Qt::darkCyan,
        Qt::darkRed
    };

    QwtPlotCurve* curve = new QwtPlotCurve;
    curve->setData(new QDaqPlotData(x,y));

    curve->setPen(QPen(QColor(eight_colors[id_++ & 0x07])));

    curve->attach(this);

    replot();

}

void QDaqPlotWidget::setTimeAxis(int axisid, bool on)
{
    if (on)
    {
        setAxisScaleEngine(axisid, new TimeScaleEngine());
        setAxisScaleDraw(axisid, new TimeScaleDraw());
    }
    else
    {
        setAxisScaleEngine(axisid, new QwtLinearScaleEngine());
        setAxisScaleDraw(axisid, new SciScaleDraw());
    }
}

void QDaqPlotWidget::clear()
{
    detachItems(QwtPlotItem::Rtti_PlotCurve,true);

    id_ = 0;

    replot();
}

QSize QDaqPlotWidget::minimumSizeHint() const
{
    return QSize(200, 130);
}
QSize QDaqPlotWidget::sizeHint() const
{
    return QSize(200, 130);
}

