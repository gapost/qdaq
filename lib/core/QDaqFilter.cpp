#include "QDaqFilter.h"
#include "QDaqChannel.h"

QDaqFilter::QDaqFilter(const QString& name) : QDaqJob(name)
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
        if (!lst.at(i)) {
            throwScriptError(QString("Null pointer in QDaqChannel list."));
            return;
        }
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        if (!ch)
        {
            throwScriptError(QString("%1 is not a QDaqChannel.").arg(lst.at(i)->objectName()));
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
        if (!lst.at(i)) {
            throwScriptError(QString("Null pointer in QDaqChannel list."));
            return;
        }
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(lst.at(i));
        if (!ch)
        {
            throwScriptError(QString("%1 is not a QDaqChannel.").arg(lst.at(i)->objectName()));
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

    bool ret = filterfunc(inbuff.constData(), outbuff.data());
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
    if (nInputChannels() != inputChannels_.size())
    {
        throwScriptError("Incorrect number of input channels.");
        return false;
    }

    if (nOutputChannels() != outputChannels_.size())
    {
        throwScriptError("Incorrect number of output channels.");
        return false;
    }

    inbuff.setCapacity(inputChannels_.size());
    outbuff.setCapacity(outputChannels_.size());

    if (!filterinit()) return false;

    return QDaqJob::arm_();
}
