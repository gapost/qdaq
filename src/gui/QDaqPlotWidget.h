#ifndef _QDaqPlotWidget_H_
#define _QDaqPlotWidget_H_

#include "QDaqGlobal.h"

#include "QDaqVector.h"

#include "QMatPlotWidget.h"

class  QDAQ_EXPORT QDaqPlotWidget : public QMatPlotWidget
{
    Q_OBJECT

public:
    explicit QDaqPlotWidget(QWidget* parent = 0);
    virtual ~QDaqPlotWidget();

public slots:
    void plot(const QDaqVector& x, const QDaqVector& y, const QString &attr, const QColor& clr = QColor());
    void plot(const QDaqVector& x, const QDaqVector& y, const QColor& clr = QColor());


};



#endif
