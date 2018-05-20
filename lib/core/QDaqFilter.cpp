#include "QDaqFilter.h"
#include "QDaqChannel.h"

#include <QDir>
#include <QApplication>
#include <QPluginLoader>

QDaqFilter::QDaqFilter(const QString& name) : QDaqJob(name),
    filter_(0)
{

}

// getters
QDaqObjectList QDaqFilter::inputChannels() const
{
    QDaqObjectList lst;
    for(int i=0; i<inputChannels_.size(); i++)
        lst.append(inputChannels_[i]);
    return lst;
}
QDaqObjectList QDaqFilter::outputChannels() const
{
    QDaqObjectList lst;
    for(int i=0; i<outputChannels_.size(); i++)
        lst.append(outputChannels_[i]);
    return lst;
}

// setters
void QDaqFilter::setInputChannels(QDaqObjectList lst)
{
    if (throwIfArmed()) return;
    // check if we have valid QDaqChannels
    for(int i=0; i<lst.size(); i++)
    {
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        if (!ch)
        {
            throwScriptError(QString("%1 is not a channel.").arg(lst.at(i)->objectName()));
            return;
        }
    }
    inputChannels_.clear();
    for(int i=0; i<lst.size(); i++)
    {
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        inputChannels_.push_back(ch);
    }
}
void QDaqFilter::setOutputChannels(QDaqObjectList lst)
{
    if (throwIfArmed()) return;
    // check if we have valid QDaqChannels
    for(int i=0; i<lst.size(); i++)
    {
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        if (!ch)
        {
            throwScriptError(QString("%1 is not a channel.").arg(lst.at(i)->objectName()));
            return;
        }
    }
    outputChannels_.clear();
    for(int i=0; i<lst.size(); i++)
    {
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        outputChannels_.push_back(ch);
    }
}

QStringList QDaqFilter::listPlugins()
{
    QStringList S;
    int idx = 0;
    QDir pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        idx++;
        //S += QString("  %1. %2: ").arg(idx).arg(fileName);
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QDaqFilterPlugin* ifilter = qobject_cast<QDaqFilterPlugin*>(plugin);
            //if (iGpib) S += "Gpib plugin";
            //else S += "Unknown plugin";
            if (ifilter) S.push_back(fileName);
        }
        //else S += loader.errorString();
    }
    return S;
}

bool QDaqFilter::loadPlugin(const QString &fname)
{
    QDir pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    QPluginLoader loader(pluginsDir.absoluteFilePath(fname));
    QObject *plugin = loader.instance();
    if (plugin) {
        QDaqFilterPlugin* ifilter = qobject_cast<QDaqFilterPlugin*>(plugin);
        if (ifilter) {
            filter_ = ifilter;
            plugin->setParent(this);
        }
    }
    return filter_!=0;
}

bool QDaqFilter::run()
{
    // get input values
    for(int i=0; i<inputChannels_.size(); i++)
    {
        QDaqChannel* ch = inputChannels_[i];
        if (ch) inbuff[i]=ch->value();
        else{
            pushError("Input channel lost.");
            return false;
        }
    }

    bool ret = (*filter_)(inbuff.constData(), outbuff.data());
    if (!ret) return false;

    // push output values
    for(int i=0; i<outputChannels_.size(); i++)
    {
        QDaqChannel* ch = outputChannels_[i];
        if (ch) ch->push(outbuff[i]);
        else{
            pushError("Output channel lost.");
            return false;
        }
    }

    return QDaqJob::run();
}

bool QDaqFilter::arm_()
{
    if (!filter_)
    {
        throwScriptError("No filter plugin loaded.");
        return false;
    }

    if (filter_->nInputChannels() != inputChannels_.size())
    {
        throwScriptError("Incorrect number of input channels.");
        return false;
    }

    if (filter_->nOutputChannels() != outputChannels_.size())
    {
        throwScriptError("Incorrect number of output channels.");
        return false;
    }

    bool ret = filter_->init();
    if (!ret)
    {
        throwScriptError(QString("Filter initialization failed: %1").arg(filter_->errorMsg()));
        return false;
    }

    inbuff.resize(inputChannels_.size());
    outbuff.resize(outputChannels_.size());

    return QDaqJob::arm_();
}
