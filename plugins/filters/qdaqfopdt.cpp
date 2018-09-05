#include "qdaqfopdt.h"

QDaqFOPDT::QDaqFOPDT(const QString& name) :
    QDaqFilter(name),
    kp_(1.),
    tp_(60),
    td_(10),
    y_(0.)
{
}

bool QDaqFOPDT::filterinit()
{
    ubuff.resize(td_);
    ubuff.fill(0.);
    ibuff = 0;

    h_ = tp_ ? exp(-1./tp_) : 1.0;

    return true;
}

bool QDaqFOPDT::filterfunc(const double* vin, double* vout)
{
    ubuff[ibuff] = *vin;
    ibuff++;
    if (ibuff>=td_) ibuff=0;
    double u = ubuff[ibuff];

    // y_ += (kp_*u-y_)/tp_;

    y_ = h_*y_ + kp_*u*(1-h_);
    *vout = y_;

    return true;
}

void QDaqFOPDT::setKp(double k)
{
    QMutexLocker L(&comm_lock);
    kp_ = k;
    emit propertiesChanged();
}
void QDaqFOPDT::setTp(uint t)
{
    QMutexLocker L(&comm_lock);
    tp_ = t;
    emit propertiesChanged();
}
void QDaqFOPDT::setTd(uint t)
{
    QMutexLocker L(&comm_lock);
    td_ = t;
    filterinit();
    emit propertiesChanged();
}
