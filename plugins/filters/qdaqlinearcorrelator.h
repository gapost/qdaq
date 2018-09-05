#ifndef QDAQLINEARCORRELATOR_H
#define QDAQLINEARCORRELATOR_H

#include "filters_global.h"

#include "QDaqFilter.h"
#include "QDaqTypes.h"

class FILTERSSHARED_EXPORT QDaqLinearCorrelator :
        public QDaqFilter
{
    Q_OBJECT

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
    Q_INVOKABLE explicit QDaqLinearCorrelator(const QString& name);

    // getters
    uint size() const { return size_; }
    uint length() const { return len_; }
    virtual int nInputChannels() const { return 2; }
    virtual int nOutputChannels() const { return 2; }

    // setters
    void setSize(uint sz);

protected:

    virtual bool filterinit();
    virtual bool filterfunc(const double* vin, double* vout);

public slots:
    void clear();
};

#endif // QDAQLINEARCORRELATOR_H
