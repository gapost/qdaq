#ifndef QDAQINTERPOLATOR_H
#define QDAQINTERPOLATOR_H

#include "interpolator_global.h"

#include "QDaqFilterPlugin.h"
#include "QDaqJob.h"
#include "QDaqTypes.h"
#include <QtPlugin>

#include <gsl/gsl_interp.h>

class INTERPOLATORSHARED_EXPORT QDaqInterpolator :
        public QDaqJob,
        public QDaqFilterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqFilterPlugin_iid FILE "interpolator.json")
    Q_INTERFACES(QDaqFilterPlugin)

    Q_PROPERTY(InterpolationType type READ type WRITE setType)
    Q_ENUMS(InterpolationType)

public:
    enum InterpolationType {
        None,
        Linear,
        Polynomial,
        CubicSpline
    };

protected:
    InterpolationType type_;
    gsl_interp* interpolator_;
    gsl_interp_accel* accel_;

    QDaqVector xa, ya;

public:
    QDaqInterpolator();
    virtual ~QDaqInterpolator();

    // QDaqFilterPlugin interface implementation
    virtual QString errorMsg() { return QString(); }
    virtual bool init();
    virtual bool operator()(const double* vin, double* vout);
    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }

    InterpolationType type() const { return type_; }
    void setType(InterpolationType t);

    virtual void registerTypes(QScriptEngine* e);

public slots:
    void setTable(const QDaqVector &x, const QDaqVector &y);
    void fromTextFile(const QString& fname);
};

#endif // QDAQINTERPOLATOR_H
