#ifndef QDAQLINEARCORRELATOR_H
#define QDAQLINEARCORRELATOR_H

#include "filters_global.h"

#include "QDaqFilterPlugin.h"
#include "QDaqJob.h"
#include "QDaqTypes.h"

class FILTERSSHARED_EXPORT QDaqLinearCorrelator :
        public QDaqJob,
        public QDaqFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(QDaqFilterPlugin)

    /** Size of data sample.
    Number of past (x,y) data points used in deriving correlation parameters.
    */
    Q_PROPERTY(uint size READ size WRITE setSize)
    /** Number of data points in buffer.
    Actual number of past (x,y) data points currently stored in the buffer.
    */
    Q_PROPERTY(uint length READ length)

    math::circular_buffer<double> x_,y_;

    uint size_, len_;

public:
    QDaqLinearCorrelator();

    // getters
    uint size() const { return size_; }
    uint length() const { return len_; }

    // setters
    void setSize(uint sz);

    // QDaqFilterPlugin interface implementation
    virtual QString iid()
    { return QString("lincorr-v0.1"); }
    virtual QString errorMsg() { return QString(); }
    virtual bool init();
    virtual bool operator()(const double* vin, double* vout);
    virtual int nInputChannels() const { return 2; }
    virtual int nOutputChannels() const { return 2; }

public slots:
    void clear();
};

#endif // QDAQLINEARCORRELATOR_H
