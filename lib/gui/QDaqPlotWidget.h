#ifndef _QDaqPlotWidget_H_
#define _QDaqPlotWidget_H_

#include "core/QDaqGlobal.h"

#include <qwt_plot.h>

class QDaqBuffer;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotPicker;

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
    Q_PROPERTY(bool grid READ grid WRITE setGrid)
    Q_PROPERTY(QPointF xlim READ xlim WRITE setXlim)
    Q_PROPERTY(QPointF ylim READ ylim WRITE setYlim)


    bool timeScaleX_, timeScaleY_, grid_on_;

    QwtPlotGrid* grid_;
    QwtPlotZoomer* zoomer;
    QwtPlotPanner* panner;
    QwtPlotPicker* picker;

    void setTimeAxis(int axisid, bool on);

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
    bool grid() const { return grid_on_; }
    QPointF xlim() const;
    QPointF ylim() const;

    //setters
    void setTitle(const QString& s);
    void setXLabel(const QString& s);
    void setYLabel(const QString& s);
    void setAutoScaleX(bool on);
    void setAutoScaleY(bool on);
    void setTimeScaleX(bool on);
    void setTimeScaleY(bool on);
    void setGrid(bool on);
    void setXlim(const QPointF& v);
    void setYlim(const QPointF& v);

	virtual QSize 	sizeHint () const	{ return QSize(10,10); }
	virtual QSize 	minimumSizeHint () const { return QSize(10,10); }

public slots:
    void plot(const QDaqBuffer& x, const QDaqBuffer& y);
    void clear();


};

#endif
