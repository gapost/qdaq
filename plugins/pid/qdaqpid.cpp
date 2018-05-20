#include "qdaqpid.h"


QDaqPid::QDaqPid() : QDaqJob("pid"),
  auto_(false),
  autotune_(false),
  Ts_(0),
  W_(0)
{
}

bool QDaqPid::init()
{
    auto_ = false;
    autotune_ = false;
    W_ = 0;
    return true;
}

bool QDaqPid::operator ()(const double* vin, double *vout)
{
    double T = *vin;

    // go through autotuner
    //bool disableCtrl;
    bool ret = tuner(Ts_, T, W_, autotune_);

    if (ret) emit propertiesChanged();

    // go through pid
    pid(Ts_, T, W_, auto_ && !autotune_);

    // send output power
    *vout = W_;

    return true;
}

// setters
void QDaqPid::setAutoMode(bool on)
{
    os::auto_lock L(comm_lock);
    auto_ = on;
    emit propertiesChanged();
}
void QDaqPid::setAutoTune(bool on)
{
    os::auto_lock L(comm_lock);
    autotune_ = on;
    emit propertiesChanged();
}
void QDaqPid::setMaxPower(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_umax(v);
    emit propertiesChanged();
}
void QDaqPid::setPower(double v)
{
    if (!auto_)
    {
        os::auto_lock L(comm_lock);
        W_ = v;
        emit propertiesChanged();
    }
}
void QDaqPid::setSamplingPeriod(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_h(v);
    emit propertiesChanged();
}
void QDaqPid::setSetPoint(double v)
{
    os::auto_lock L(comm_lock);
    Ts_ = v;
    emit propertiesChanged();
}
void QDaqPid::setGain(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_k(v);
    emit propertiesChanged();
}
void QDaqPid::setTi(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_ti(v);
    emit propertiesChanged();
}
void QDaqPid::setTd(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_td(v);
    emit propertiesChanged();
}
void QDaqPid::setTr(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_tr(v);
    emit propertiesChanged();
}
void QDaqPid::setNd(uint v)
{
    os::auto_lock L(comm_lock);
    pid.set_N(v);
    emit propertiesChanged();
}
void QDaqPid::setBeta(double v)
{
    os::auto_lock L(comm_lock);
    pid.set_b(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayStep(double v)
{
    os::auto_lock L(comm_lock);
    tuner.set_step(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayOffset(double v)
{
    v=v;
}
void QDaqPid::setRelayThreshold(double v)
{
    os::auto_lock L(comm_lock);
    tuner.set_dy(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayIterations(int v)
{
    os::auto_lock L(comm_lock);
    tuner.set_count(v);
    emit propertiesChanged();
}
