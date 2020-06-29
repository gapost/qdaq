#include "QDaqPlotWidget.h"

#include "QDaqTypes.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QSet>
#include <QRegularExpression>

//#include <qwt_plot_curve.h>
#include <qwt_plot_dict.h>
#include <qwt_series_data.h>
#include <qwt_symbol.h>
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
    virtual QwtText trackerTextF	(	const QPointF & 	pos	 ) const
    {
    //Since the "paintAttributes", [text+background colour] act on QwtTexts
    //break up the creation of trackerTextF: one function to create the text
    //(as a QString), and another to set attributes and return the object.
    QString S = createLabelText(pos);
    QwtText trackerText;
    trackerText.setBackgroundBrush(Qt::lightGray);
    trackerText.setText(S);
    trackerText.setColor(Qt::black);
    return trackerText;
    }
    QString createLabelText (const QPointF & pos) const
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
    QDaqVector vx;
    QDaqVector vy;
    size_t sz;
public:
    QDaqPlotData(const QDaqVector& x, const QDaqVector& y) : vx(x), vy(y)
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
    QDaqVector& returnVx() {return vx;}
    QDaqVector& returnVy() {return vy;}

    virtual QRectF boundingRect() const
    {
        const_cast<QDaqPlotData*>(this)->sz = qMin(vx.size(),vy.size());
        double x1 = vx.vmin(), x2 = vx.vmax();
        double y1 = vy.vmin(), y2 = vy.vmax();
        return QRectF(x1,y1,x2-x1,y2-y1);
    }

    void update(const QDaqPlotWidget* w, const QDaqVector* v)
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
    logScaleX_(false),  logScaleY_(false),
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

QwtPlotCurve * QDaqPlotWidget::getCurve() const
{

    int curves = 0;
    QwtPlotCurve * curve = new QwtPlotCurve;
    QwtPlotItemList  plotItemList = this->itemList();
    foreach (QwtPlotItem * plotItem, plotItemList)
    {
         int rtt = plotItem->rtti();
         if (rtt == QwtPlotItem::Rtti_PlotCurve){
//        if (rtt == 5){
        curve = (QwtPlotCurve*)plotItem;
        curves++;
        }
    }
//    qInfo ("number of curves = %d", curves);

    return curve;


};

int QDaqPlotWidget::getNumberOfCurves() const
{
    int curves = 0;
    QwtPlotItemList  plotItemList = this->itemList();
    foreach (QwtPlotItem * plotItem, plotItemList)
    {
         int rtt = plotItem->rtti();
         if (rtt == QwtPlotItem::Rtti_PlotCurve){
        curves++;
        }
    }
    return curves;
}

QwtPlotItemList  QDaqPlotWidget::getCurves() const
{
    int curves = 0;
    QwtPlotCurve * curve = new QwtPlotCurve;
    QwtPlotItemList  plotItemList = this->itemList();
    QwtPlotItemList  plotItemList2;
    foreach (QwtPlotItem * plotItem, plotItemList)
    {
         int rtt = plotItem->rtti();
         if (rtt == QwtPlotItem::Rtti_PlotCurve){
        curve = (QwtPlotCurve*)plotItem;
        plotItemList2.append(curve);
        curves++;
        }
    }
//    qInfo ("number of curves = %d", curves);

    return plotItemList2;

}


QwtPlotCurve * QDaqPlotWidget::getItem() const
//QwtPlotItem * QDaqPlotWidget::getItem() const
{
//    int ind;
    QwtPlotCurve * recurves = new QwtPlotCurve;
    QwtPlotItemList  plotItemList = this->itemList();
    foreach (QwtPlotItem * plotItem, plotItemList)
    {
         int rtt = plotItem->rtti();
         if (rtt == QwtPlotItem::Rtti_PlotCurve){
//             if (rtt == 5){
//            ind = plotItemList.indexOf(plotItem);
            recurves = (QwtPlotCurve *)plotItem;
        }
    }
    return recurves;
//    return plotItemList[ind];
};


void QDaqPlotWidget::changeStyle(QString attr)
{
    //Get QwPlotCurve objects
    QwtPlotItemList recurves = this->getCurves();
    //object to hold one curve
    QwtPlotCurve* recurve = new QwtPlotCurve;
    //loop over curves
    foreach (QwtPlotItem * plotItem, recurves)
    {
            recurve = (QwtPlotCurve *)plotItem;
            //get curve's data
            QDaqPlotData* myData = (QDaqPlotData*)recurve->data();

            //object to hold data from current curve
            QDaqVector x,y;

            // select existing colour. If "none" linestyle (no line, only markers)
            // had been selected in the previous call to changeStyle,
            // the plot will revert to black:[ clr.name() = "#000000"]
            QColor clr = recurve->pen().color();

            // small function reColor() is a kludge way to keep the original
            // color scheme for multiple (TODO: more than 2!) plotCurves
            if (clr.name()=="#000000") {clr = reColor();}

            //remove previous curve & markers
            //this is safer: delete only one (current) data series
            recurve->detach();

            //recreate the QDaqVectors
            x = myData->returnVx();
            y = myData->returnVy();

            //replot
            this->plot(x,y,attr,clr);
    }
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
    if (on && logScaleX()) setLogScaleX(false);
    setTimeAxis(QwtPlot::xBottom, on);
    timeScaleX_ = on;
}
void QDaqPlotWidget::setTimeScaleY(bool on)
{
    if (on==timeScaleY_) return;
    if (on && logScaleY()) setLogScaleY(false);
    setTimeAxis(QwtPlot::yLeft, on);
    timeScaleY_ = on;
}
void QDaqPlotWidget::setLogScaleX(bool on)
{
    if (on==logScaleX_) return;
    if (on && timeScaleX()) setTimeScaleX(false);
    setLogAxis(QwtPlot::xBottom, on);
    logScaleX_ = on;
}
void QDaqPlotWidget::setLogScaleY(bool on)
{
    if (on==logScaleY_) return;
    if (on && timeScaleY()) setTimeScaleY(false);
    setLogAxis(QwtPlot::yLeft, on);
    logScaleY_ = on;
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

void QDaqPlotWidget::plot(const QDaqVector &x, const QDaqVector &y, const QString &attr, const QColor &clr)
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

    //MATLAB-Octave-style linestyles for reference
    //static const QString linestyles[5] =
    //{"-","--",":",".-","none"};

    //Respective Qwt names for QwtPlotCurve::setPen Style: (a Qt::PenStyle object)
    static const Qt::PenStyle penstyles[4] =
    {Qt::SolidLine,Qt::DashLine,Qt::DotLine,Qt::DashDotLine};

    //MATLAB-Octave-style markerstyles for reference (not all)
    //static const QString markerstyles[12] =
    //{"+","o","*",".","x","s","d","^","v",">","<","h"};

    //Respective Qwt names for QwtSymbol::Style
    static const QwtSymbol::Style dotstyles[12] =
    {QwtSymbol::Cross,QwtSymbol::Ellipse,QwtSymbol::Star1,QwtSymbol::NoSymbol,QwtSymbol::XCross,
     QwtSymbol::Rect,QwtSymbol::Diamond,QwtSymbol::UTriangle,QwtSymbol::DTriangle,QwtSymbol::RTriangle,
     QwtSymbol::LTriangle,QwtSymbol::Hexagon};

    QwtPlotCurve* curve = new QwtPlotCurve;
    curve->setData(new QDaqPlotData(x,y));

    QColor plotclr = (clr.isValid()) ? clr : QColor(eight_colors[id_++ & 0x07]);

    QString lines, markers;

    QString pattern("(?<line>(-|--|:|.-|none))(?<marker>(\\+|o|\\*|\\.|x|s|d|\\^|v|>|<|h))");
    //use the pattern as a regular expression
    QRegularExpression re(pattern);
    //try to match the plot attributes passed by the user to the reg expr
    QRegularExpressionMatch match = re.match(attr,0);

    QwtSymbol * symbol = new QwtSymbol;
    //set a discernible but not very intrusive size for markers
    symbol->setSize(4,4);
    //make the marker same color as plot
    symbol->setColor(plotclr);

    //if we find a match in user input and pattern, separate the two fields of match
    if (match.hasMatch()) {
        lines = match.captured("line");
        markers = match.captured("marker");
    }
    else
    {
        //error message, instructions for proper usage, select default plot !!!!
        //qInfo ("Please give linestyle/markerstyle attributes, following MATLAB style");
        //qInfo ("I will now select some default: cont. lines, no marker");
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setSymbol(NULL);
    }
//    qInfo ("attr 2 = %s", qUtf8Printable(attr));
//    qInfo ("lines = %s", qUtf8Printable(lines));
//    qInfo ("markers = %s", qUtf8Printable(markers));


    //Ok, now select linestyle
    if (lines == "none")
    {
        //if user desires no line, at least plot markers, instead of nothing!
        curve->setStyle(QwtPlotCurve::Dots);
    }
    else if (lines == "-")
    {
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setPen(plotclr, 0.0, penstyles[0]);
    }
    else if (lines == "--")
    {
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setPen(plotclr, 0.0, penstyles[1]);
    }
    else if (lines == ":")
    {
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setPen(plotclr, 0.0, penstyles[2]);
    }
    else if (lines == ".-")
    {
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setPen(plotclr, 0.0, penstyles[3]);
    }
    else
    {
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setPen(plotclr, 0.0, penstyles[0]);
    }

//Now on for the marker style...
    if (markers == ".")
    {
        //if user desires no marker, indulge them: plot only a line
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setSymbol(NULL);
    }
    else if (markers == "+")
    {
        symbol->setStyle(dotstyles[0]);
    }
    else if (markers == "o")
    {
        symbol->setStyle(dotstyles[1]);
    }
    else if (markers == "*")
    {
        symbol->setStyle(dotstyles[2]);
    }
    else if (markers == "x")
    {
        symbol->setStyle(dotstyles[4]);
    }
    else if (markers == "s")
    {
        symbol->setStyle(dotstyles[5]);
    }
    else if (markers == "d")
    {
        symbol->setStyle(dotstyles[6]);
    }
    else if (markers == "^")
    {
        symbol->setStyle(dotstyles[7]);
    }
    else if (markers == "v")
    {
        symbol->setStyle(dotstyles[8]);
    }
    else if (markers == ">")
    {
        symbol->setStyle(dotstyles[9]);
    }
    else if (markers == "<")
    {
        symbol->setStyle(dotstyles[10]);
    }
    else if (markers == "h")
    {
        symbol->setStyle(dotstyles[11]);
    }
    else{
        symbol->setStyle(dotstyles[3]);
    }
    curve->setSymbol(symbol);

    curve->attach(this);

    replot();
}

QColor QDaqPlotWidget::reColor(const QColor &clr)
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
    // The number to be added to id_ should be equal to the number of plotCurves-1(?),
    // in order to re-create the original color scheme (but id_ should also be zeroed!).
    // I kept it at 1 since most plots in ir2app have at most 2 plotCurves.
//    int curves  = this->getNumberOfCurves()-1;
    QColor plotclr = (clr.isValid()) ? clr : QColor(eight_colors[id_++ & 0x01]);
//    QColor plotclr = (clr.isValid()) ? clr : QColor(eight_colors[id_++ & curves]);

    return plotclr;
}
void QDaqPlotWidget::plot(const QDaqVector &x, const QDaqVector &y, const QColor &clr)
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
    QColor plotclr = (clr.isValid()) ? clr : QColor(eight_colors[id_++ & 0x07]);

    curve->setPen(plotclr);

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

void QDaqPlotWidget::setLogAxis(int axisid, bool on)
{
    if (on)
    {
        setAxisScaleEngine(axisid, new QwtLogScaleEngine());
        setAxisScaleDraw(axisid, new SciScaleDraw());
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

