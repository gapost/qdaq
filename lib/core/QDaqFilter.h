#ifndef QDAQFILTER_H
#define QDAQFILTER_H

#include "QDaqJob.h"
#include "QDaqTypes.h"

#include <QPointer>

class QDaqChannel;

class QDAQ_EXPORT QDaqFilter : public QDaqJob
{
    Q_OBJECT

    /// Number of input channels for this filter.
    Q_PROPERTY(int nInputChannels READ nInputChannels)
    /// Number of output channels for this filter.
    Q_PROPERTY(int nOutputChannels READ nOutputChannels)
    /// A QList of the input channels for this filter.
    Q_PROPERTY(QDaqObjectList inputChannels READ inputChannels WRITE setInputChannels)
    /// A QList of the output channels for this filter.
    Q_PROPERTY(QDaqObjectList outputChannels READ outputChannels WRITE setOutputChannels)

    // typedefs of channel ptr, channel vector, matrix
    typedef QPointer<QDaqChannel> channel_t;
    typedef QVector<channel_t> channel_vector_t;

    channel_vector_t inputChannels_, outputChannels_;
    QDaqVector inbuff, outbuff;

public:    
    explicit QDaqFilter(const QString& name);

    // getters
    virtual int nInputChannels() const = 0;
    virtual int nOutputChannels() const = 0;
    QDaqObjectList inputChannels() const;
    QDaqObjectList outputChannels() const;

    // setters
    void setInputChannels(QDaqObjectList lst);
    void setOutputChannels(QDaqObjectList lst);

protected:
    virtual bool arm_();
    virtual bool run();

    virtual bool filterinit() = 0; // { return false; }
    virtual bool filterfunc(const double* in, double* out) = 0;
    /*{
        Q_UNUSED(in);
        Q_UNUSED(out);
        return false;
    }*/

};

#endif // QDAQFILTER_H
