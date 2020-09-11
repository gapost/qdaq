#ifndef QDAQINTERPOLATOR_H
#define QDAQINTERPOLATOR_H

#include "QDaqGlobal.h"

#include "QDaqFilter.h"
#include "QDaqVector.h"

#include <gsl/gsl_interp.h>

class QDAQ_EXPORT QDaqInterpolator :
        public QDaqFilter
{
    Q_OBJECT

    Q_PROPERTY(InterpolationType type READ type WRITE setType)
    Q_PROPERTY(QDaqVector x READ x)
    Q_PROPERTY(QDaqVector y READ y)


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
    Q_INVOKABLE explicit QDaqInterpolator(const QString& name);
    virtual ~QDaqInterpolator();

    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }

    InterpolationType type() const { return type_; }
    void setType(InterpolationType t);

    QDaqVector x() const;
    QDaqVector y() const;

protected:
    virtual bool filterinit();
    virtual bool filterfunc(const double* vin, double* vout);

public slots:
    void setTable(const QDaqVector &x, const QDaqVector &y);
    void fromTextFile(const QString& fname);
};

#endif // QDAQINTERPOLATOR_H
