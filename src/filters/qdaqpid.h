#ifndef QDAQPID_H
#define QDAQPID_H

#include "QDaqGlobal.h"

#include "QDaqFilter.h"
#include <QtPlugin>

#include "isa_pid.h"
#include "relaytuner.h"

class QDAQ_EXPORT QDaqPid :
        public QDaqFilter
{
    Q_OBJECT

    Q_PROPERTY(double samplingPeriod READ samplingPeriod WRITE setSamplingPeriod)
    Q_PROPERTY(double maxPower READ maxPower WRITE setMaxPower)
    Q_PROPERTY(double power READ power WRITE setPower)
    Q_PROPERTY(bool autoMode READ autoMode WRITE setAutoMode)
    Q_PROPERTY(double setPoint READ setPoint WRITE setSetPoint)
    Q_PROPERTY(double gain READ gain WRITE setGain)
    Q_PROPERTY(double Ti READ Ti WRITE setTi)
    Q_PROPERTY(double Td READ Td WRITE setTd)
    Q_PROPERTY(double Tr READ Tr WRITE setTr)
    Q_PROPERTY(uint Nd READ Nd WRITE setNd)
    Q_PROPERTY(double beta READ beta WRITE setBeta)
    Q_PROPERTY(bool autoTune READ autoTune WRITE setAutoTune)
    Q_PROPERTY(double relayStep READ relayStep WRITE setRelayStep)
    Q_PROPERTY(double relayOffset READ relayOffset WRITE setRelayOffset)
    Q_PROPERTY(double relayThreshold READ relayThreshold WRITE setRelayThreshold)
    Q_PROPERTY(int relayIterations READ relayIterations WRITE setRelayIterations)
    Q_PROPERTY(double Kc READ Kc)
    Q_PROPERTY(double Tc READ Tc)

protected:
    bool auto_, autotune_;
    double Ts_, W_;

    isa_pid<double> pid;
    autotuner<double> tuner;

public:
    Q_INVOKABLE explicit QDaqPid(const QString& name);


    // getters
    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }
    bool autoMode() const { return auto_; }
    double maxPower() const { return pid.get_umax(); }
    double power() const { return W_; }
    double samplingPeriod() const { return pid.get_h(); }
    double setPoint() const { return Ts_; }
    double gain() const { return pid.get_k(); }
    double Ti() const { return pid.get_ti(); }
    double Td() const { return pid.get_td(); }
    double Tr() const { return pid.get_tr(); }
    uint Nd() const { return pid.get_N(); }
    double beta() const { return pid.get_b(); }
    double relayStep() const { return tuner.get_step(); }
    double relayOffset() const { return 0; }
    double relayThreshold() const { return tuner.get_dy(); }
    int relayIterations() const { return tuner.get_count(); }
    double Kc() const { return tuner.get_kc(); }
    double Tc() const { return tuner.get_tc()*pid.get_h(); }
    bool autoTune() const { return autotune_; }

    // setters
    void setAutoMode(bool on);
    void setMaxPower(double v);
    void setPower(double v);
    void setSamplingPeriod(double v);
    void setSetPoint(double v);
    void setGain(double v);
    void setTi(double v);
    void setTd(double v);
    void setTr(double v);
    void setNd(uint v);
    void setBeta(double v);
    void setRelayStep(double v);
    void setRelayOffset(double v);
    void setRelayThreshold(double v);
    void setRelayIterations(int v);
    void setAutoTune(bool on);

protected:
    virtual bool filterinit();
    virtual bool filterfunc(const double* vin, double* vout);
};

Q_DECLARE_METATYPE(QDaqPid*)

#endif // QDAQPID_H
