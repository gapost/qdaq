#include "qdaqlinearcorrelator.h"
#include "linefit.h"

QDaqLinearCorrelator::QDaqLinearCorrelator(const QString& name) :
    QDaqFilter(name),
    size_(2),
    len_(0)
{
    x_.alloc(2);
    y_.alloc(2);
}

void QDaqLinearCorrelator::setSize(uint sz)
{
    if ((sz!=size()) && sz>1)
    {
        {
            QMutexLocker L(&comm_lock);
            x_.alloc(sz);
            y_.alloc(sz);
            size_ = sz;
            len_ = 0;
        }
        emit propertiesChanged();
    }
}

bool QDaqLinearCorrelator::filterinit()
{
    len_ = 0;
    return true;
}

bool QDaqLinearCorrelator::filterfunc(const double* vin, double* vout)
{
    x_ << vin[0];
    y_ << vin[1];
    if (len_ < size_) len_++;
    if (len_>1)
    {
        linefit<double> fitobj(x_,y_,len_);
        vout[0] = fitobj.a;
        vout[1] = fitobj.b;
    } else vout[0] = vout[1] = 0.;

    return true;
}

void QDaqLinearCorrelator::clear()
{
    QMutexLocker L(&comm_lock);
    len_ = 0;
}
