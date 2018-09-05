#include "qdaqpid.h"


QDaqPid::QDaqPid(const QString& name) :
    QDaqFilter(name),
    auto_(false),
    autotune_(false),
    Ts_(0),
    W_(0)
{
}

bool QDaqPid::filterinit()
{
    auto_ = false;
    autotune_ = false;
    W_ = 0;
    return true;
}

bool QDaqPid::filterfunc(const double* vin, double *vout)
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
    QMutexLocker L(&comm_lock);
    auto_ = on;
    emit propertiesChanged();
}
void QDaqPid::setAutoTune(bool on)
{
    QMutexLocker L(&comm_lock);
    autotune_ = on;
    emit propertiesChanged();
}
void QDaqPid::setMaxPower(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_umax(v);
    emit propertiesChanged();
}
void QDaqPid::setPower(double v)
{
    if (!auto_)
    {
        QMutexLocker L(&comm_lock);
        W_ = v;
        emit propertiesChanged();
    }
}
void QDaqPid::setSamplingPeriod(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_h(v);
    emit propertiesChanged();
}
void QDaqPid::setSetPoint(double v)
{
    QMutexLocker L(&comm_lock);
    Ts_ = v;
    emit propertiesChanged();
}
void QDaqPid::setGain(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_k(v);
    emit propertiesChanged();
}
void QDaqPid::setTi(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_ti(v);
    emit propertiesChanged();
}
void QDaqPid::setTd(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_td(v);
    emit propertiesChanged();
}
void QDaqPid::setTr(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_tr(v);
    emit propertiesChanged();
}
void QDaqPid::setNd(uint v)
{
    QMutexLocker L(&comm_lock);
    pid.set_N(v);
    emit propertiesChanged();
}
void QDaqPid::setBeta(double v)
{
    QMutexLocker L(&comm_lock);
    pid.set_b(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayStep(double v)
{
    QMutexLocker L(&comm_lock);
    tuner.set_step(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayOffset(double v)
{
    v=v;
}
void QDaqPid::setRelayThreshold(double v)
{
    QMutexLocker L(&comm_lock);
    tuner.set_dy(v);
    emit propertiesChanged();
}
void QDaqPid::setRelayIterations(int v)
{
    QMutexLocker L(&comm_lock);
    tuner.set_count(v);
    emit propertiesChanged();
}
