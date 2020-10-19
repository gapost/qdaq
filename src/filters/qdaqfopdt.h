#ifndef QDAQFOPDT_H
#define QDAQFOPDT_H

#include "QDaqGlobal.h"

#include "QDaqFilter.h"
#include "QDaqVector.h"

class QDAQ_EXPORT QDaqFOPDT :
        public QDaqFilter
{
    Q_OBJECT

    Q_PROPERTY(double kp READ kp WRITE setKp)
    Q_PROPERTY(uint tp READ tp WRITE setTp)
    Q_PROPERTY(uint td READ td WRITE setTd)


    double kp_;
    uint tp_, td_;
    double y_;


    double h_;

    QDaqVector ubuff;
    uint ibuff;

public:
    Q_INVOKABLE explicit QDaqFOPDT(const QString& name);

    // getters
    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }
    double kp() const { return kp_; }
    uint tp() const { return tp_; }
    uint td() const { return td_; }

    // setters
    void setKp(double k);
    void setTp(uint t);
    void setTd(uint t);

protected:
    virtual bool filterinit();
    virtual bool filterfunc(const double* vin, double* vout);


};

Q_DECLARE_METATYPE(QDaqFOPDT*)

#endif // QDAQFOPDT_H
