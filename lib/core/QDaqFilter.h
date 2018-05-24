#ifndef QDAQFILTER_H
#define QDAQFILTER_H

#include "QDaqJob.h"
#include "QDaqTypes.h"

#include "QDaqFilterPlugin.h"

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

    // the filter
    QDaqFilterPlugin* filter_;
    QDaqObject* filterWrapper_;

    // typedefs of channel ptr, channel vector, matrix
    typedef QPointer<QDaqChannel> channel_t;
    typedef QVector<channel_t> channel_vector_t;

    channel_vector_t inputChannels_, outputChannels_;
    QDaqVector inbuff, outbuff;

public:    
    Q_INVOKABLE explicit QDaqFilter(const QString& name);

    // getters
    int nInputChannels() const { return filter_ ? filter_->nInputChannels() : 0; }
    int nOutputChannels() const { return filter_ ? filter_->nOutputChannels() : 0; }
    QDaqObjectList inputChannels() const;
    QDaqObjectList outputChannels() const;

    // setters
    void setInputChannels(QDaqObjectList lst);
    void setOutputChannels(QDaqObjectList lst);

public slots:
    /**
     * @brief Return a list of available filter plugins.
     * For each plugin the file name is returned, which
     * can be passed to loadPlugin().
     * @return A string list of file names.
     */
    QStringList listPlugins();
    /**
     * @brief Loads the filter plugin specified by fname.
     * @param fname File name of required plugin.
     * @return True if the plugin is sucessfully loaded.
     */
    bool loadPlugin(const QString& fname);

protected:
    virtual bool arm_();
    virtual bool run();

};

#endif // QDAQFILTER_H
