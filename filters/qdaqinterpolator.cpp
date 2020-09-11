#include "qdaqinterpolator.h"

#include <QFile>
#include <QTextStream>

static const gsl_interp_type* InterpolationObjects[] = {
    0,
    gsl_interp_linear,
    gsl_interp_polynomial,
    gsl_interp_cspline
};

QDaqInterpolator::QDaqInterpolator(const QString& name) :
    QDaqFilter(name),
    type_(None),
    interpolator_(0),
    accel_(0)
{
    accel_ = gsl_interp_accel_alloc ();
}

QDaqInterpolator::~QDaqInterpolator(void)
{
    if (interpolator_) gsl_interp_free (interpolator_);
    gsl_interp_accel_free (accel_);
}

bool QDaqInterpolator::filterinit()
{
    if (interpolator_) gsl_interp_free (interpolator_);
    interpolator_ = 0;
    int it = (int)type_;
    unsigned int n = xa.size();
    if (it>0 && n>1)
    {        
        interpolator_ = gsl_interp_alloc(InterpolationObjects[it],n);
        gsl_interp_init(interpolator_, xa.constData(), ya.constData(), n);
    }
    return true;
}

bool QDaqInterpolator::filterfunc(const double* vin, double* vout)
{
    if (interpolator_)
    {
        double val;
        int ret = gsl_interp_eval_e(interpolator_, xa.constData(), ya.constData(), *vin, accel_, &val);
        *vout = (ret==0) ? val : *vin;
    }
    else *vout = *vin;

    return true;
}

void QDaqInterpolator::setType(InterpolationType t)
{
    if (!throwIfArmed() && type_ != t && (int)t!=-1)
    {
        type_ = t;
        emit propertiesChanged();
    }
}

QDaqVector QDaqInterpolator::x() const
{
    return xa.clone();
}

QDaqVector QDaqInterpolator::y() const
{
    return ya.clone();
}

void QDaqInterpolator::setTable(const QDaqVector& x, const QDaqVector& y)
{
    if (throwIfArmed()) return;

    int n = qMin(x.size(), y.size());
    if (n<2)
    {
        throwScriptError("Vectors must be at least of length 2.");
        return;
    }

    // check that x is ordered
    double d0 = x[1]-x[0];
    bool ok = true;
    for(int i=2; i<n; ++i)
    {
        if (d0*(x[i]-x[i-1])<=0.0) { ok=false; break; }
    }
    if (!ok)
    {
        throwScriptError("x vector must be ordered.");
        return;
    }

    // copy the data
    xa.setSize(n);
    ya.setSize(n);
    if (d0>0.0)
    {
        double *px = xa.data();
        double *py = ya.data();
        for(int i=0; i<n; ++i)
        {
            *px++ = x[i]; *py++ = y[i];
        }
    }
    else
    {
        double *px = xa.data() + n - 1;
        double *py = ya.data() + n - 1;
        for(int i=0; i<n; ++i)
        {
            *px-- = x[i]; *py-- = y[i];
        }
    }
}

void QDaqInterpolator::fromTextFile(const QString& fname)
{
    if (throwIfArmed()) return;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throwScriptError("File cannot be read.");
        return;
    }


    QTextStream in(&file);
    QVector<double> x;
    bool ok = true;
    while (ok && !in.atEnd() && in.status()==QTextStream::Ok)
    {
        QString str;
        in >> str;
        double d = str.toDouble(&ok);
        if (ok) x.push_back(d);
    }

    int n = x.size();
    if (n<4)
    {
        throwScriptError("File does not contain enough data.");
        return;
    }

    // # of pairs
    if (n & 0x01) n--;
    n>>=1;

    // check if x ordered
    double d = x[2]-x[0];
    ok = true;
    for(int i=2; i<n; ++i)
    {
        int j=2*i;
        if (d*(x[j]-x[j-2])<=0.0) { ok=false; break; }
    }
    if (!ok)
    {
        throwScriptError("x vector must be ordered.");
        return;
    }


    // copy the data
    xa.setSize(n);
    ya.setSize(n);
    if (d>0.0)
    {
        double *px = xa.data();
        double *py = ya.data();
        const double *p = x.constData();
        for(int i=0; i<n; ++i)
        {
            *px++ = *p++;
            *py++ = *p++;
        }
    }
    else
    {
        double *px = xa.data() + n - 1;
        double *py = ya.data() + n - 1;
        const double *p = x.constData();
        for(int i=0; i<n; ++i)
        {
            *px-- = *p++;
            *py-- = *p++;
        }
    }
}
