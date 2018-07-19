#include "qdaqfopdt.h"

QDaqFOPDT::QDaqFOPDT() :
    QDaqJob("fopdt"),
    kp_(1.),
    tp_(60),
    td_(10),
    y_(0.)
{
}

bool QDaqFOPDT::init()
{
    ubuff.resize(td_);
    ubuff.fill(0.);
    ibuff = 0;

    h_ = tp_ ? exp(-1./tp_) : 1.0;

    return true;
}

bool QDaqFOPDT::operator ()(const double* vin, double* vout)
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
    os::auto_lock L(comm_lock);
    kp_ = k;
    emit propertiesChanged();
}
void QDaqFOPDT::setTp(uint t)
{
    os::auto_lock L(comm_lock);
    tp_ = t;
    emit propertiesChanged();
}
void QDaqFOPDT::setTd(uint t)
{
    os::auto_lock L(comm_lock);
    td_ = t;
    init();
    emit propertiesChanged();
}
