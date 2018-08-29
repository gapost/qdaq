#ifndef QDAQINTERPOLATOR_H
#define QDAQINTERPOLATOR_H

#include "filters_global.h"

#include "QDaqFilterPlugin.h"
#include "QDaqJob.h"
#include "QDaqTypes.h"

#include <gsl/gsl_interp.h>

class FILTERSSHARED_EXPORT QDaqInterpolator :
        public QDaqJob,
        public QDaqFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(QDaqFilterPlugin)

    Q_PROPERTY(InterpolationType type READ type WRITE setType)


public:
    enum InterpolationType {
        None,
        Linear,
        Polynomial,
        CubicSpline
    };
    Q_ENUM(InterpolationType)

protected:
    InterpolationType type_;
    gsl_interp* interpolator_;
    gsl_interp_accel* accel_;

    QDaqVector xa, ya;

public:
    QDaqInterpolator();
    virtual ~QDaqInterpolator();

    // QDaqFilterPlugin interface implementation
    virtual QString iid()
    { return QString("interp-v0.1"); }
    virtual QString errorMsg() { return QString(); }
    virtual bool init();
    virtual bool operator()(const double* vin, double* vout);
    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }

    InterpolationType type() const { return type_; }
    void setType(InterpolationType t);

public slots:
    void setTable(const QDaqVector &x, const QDaqVector &y);
    void fromTextFile(const QString& fname);
};

#endif // QDAQINTERPOLATOR_H
