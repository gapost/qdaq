#ifndef _QDaqPlotWidget_H_
#define _QDaqPlotWidget_H_

#include "QDaqGlobal.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class QDaqVector;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotPicker;
Q_DECLARE_METATYPE( QwtPlotCurve * )
class  QDAQ_EXPORT QDaqPlotWidget : public QwtPlot
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString xlabel READ xlabel WRITE setXLabel)
    Q_PROPERTY(QString ylabel READ ylabel WRITE setYLabel)
    Q_PROPERTY(bool autoScaleX READ autoScaleX WRITE setAutoScaleX)
    Q_PROPERTY(bool autoScaleY READ autoScaleY WRITE setAutoScaleY)
    Q_PROPERTY(bool timeScaleX READ timeScaleX WRITE setTimeScaleX)
    Q_PROPERTY(bool timeScaleY READ timeScaleY WRITE setTimeScaleY)
    Q_PROPERTY(bool logScaleX READ logScaleX WRITE setLogScaleX)
    Q_PROPERTY(bool logScaleY READ logScaleY WRITE setLogScaleY)
    Q_PROPERTY(bool grid READ grid WRITE setGrid)
    Q_PROPERTY(QPointF xlim READ xlim WRITE setXlim)
    Q_PROPERTY(QPointF ylim READ ylim WRITE setYlim)
    Q_PROPERTY(QwtPlotCurve* plotcurve READ getCurve)
//    Q_PROPERTY(QwtPlotItem* plotitem READ getItem WRITE setItem)
    Q_PROPERTY(QwtPlotItem* plotitem READ getItem)


    bool timeScaleX_, timeScaleY_,
        logScaleX_, logScaleY_, grid_on_;

    int id_;

    QwtPlotGrid* grid_;
    QwtPlotZoomer* zoomer;
    QwtPlotPanner* panner;
    QwtPlotPicker* picker;

 //       QwtPlotCurve* plotcurve_;
 //       QwtPlotItemList plotitem_;

    void setTimeAxis(int axisid, bool on);
    void setLogAxis(int axisid, bool on);

public:
    explicit QDaqPlotWidget(QWidget* parent = 0);
    virtual ~QDaqPlotWidget();

    //getters
    QString title() const;
    QString xlabel() const;
    QString ylabel() const;
    bool autoScaleX() const;
    bool autoScaleY() const;
    bool timeScaleX() const { return timeScaleX_; }
    bool timeScaleY() const { return timeScaleY_; }
    bool logScaleX() const { return logScaleX_; }
    bool logScaleY() const { return logScaleY_; }
    bool grid() const { return grid_on_; }
    QPointF xlim() const;
    QPointF ylim() const;

    QwtPlotCurve * getCurve() const;
//    QwtPlotItem * getItem() const;
    QwtPlotCurve * getItem() const;
    QwtPlotItemList  getCurves() const;
    int getNumberOfCurves() const;


    //setters
    void setTitle(const QString& s);
    void setXLabel(const QString& s);
    void setYLabel(const QString& s);
    void setAutoScaleX(bool on);
    void setAutoScaleY(bool on);
    void setTimeScaleX(bool on);
    void setTimeScaleY(bool on);
    void setLogScaleX(bool on);
    void setLogScaleY(bool on);
    void setGrid(bool on);
    void setXlim(const QPointF& v);
    void setYlim(const QPointF& v);

    virtual QSize 	sizeHint () const;
    virtual QSize 	minimumSizeHint () const;

//    void setCurve(QwtPlotCurve* curve){ return;};
//    void setItem(QwtPlotItem * item){ return;};
public slots:
    void plot(const QDaqVector& x, const QDaqVector& y, const QString &attr, const QColor& clr = QColor());
    void plot(const QDaqVector& x, const QDaqVector& y, const QColor& clr = QColor());
    void changeStyle(QString attr);
    QColor reColor(const QColor &clr = QColor());
    void clear();


};

#endif
