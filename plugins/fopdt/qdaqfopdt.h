#ifndef QDAQFOPDT_H
#define QDAQFOPDT_H

#include "fopdt_global.h"

#include "QDaqFilterPlugin.h"
#include "QDaqJob.h"
#include "QDaqTypes.h"
#include <QtPlugin>

class FOPDTSHARED_EXPORT QDaqFOPDT :
        public QDaqJob,
        public QDaqFilterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqFilterPlugin_iid FILE "fopdt.json")
    Q_INTERFACES(QDaqFilterPlugin)

    Q_PROPERTY(double kp READ kp WRITE setKp)
    Q_PROPERTY(uint tp READ tp WRITE setTp)
    Q_PROPERTY(uint td READ td WRITE setTd)

    double kp_;
    uint tp_, td_;
    double y_;

    QDaqVector ubuff;
    uint ibuff;

public:
    QDaqFOPDT();

    // getters
    double kp() const { return kp_; }
    uint tp() const { return tp_; }
    uint td() const { return td_; }

    // setters
    void setKp(double k);
    void setTp(uint t);
    void setTd(uint t);

    // QDaqFilterPlugin interface implementation
    virtual QString errorMsg() { return QString(); }
    virtual bool init();
    virtual bool operator()(const double* vin, double* vout);
    virtual int nInputChannels() const { return 1; }
    virtual int nOutputChannels() const { return 1; }

};

#endif // QDAQFOPDT_H
