#include "QDaqDataBuffer.h"
#include "QDaqChannel.h"

#include <QCoreApplication>
#include <QVariant>

#include "QDaqEnumHelper.h"

Q_SCRIPT_ENUM(BufferType,QDaqDataBuffer)

void QDaqDataBuffer::registerTypes(QScriptEngine* e)
{
	qScriptRegisterBufferType(e);
    QDaqJob::registerTypes(e);
}

QDaqDataBuffer::QDaqDataBuffer(const QString &name) : QDaqJob(name),
	type_(Open)
{
    connect(this,SIGNAL(dataReady()),this,SLOT(onDataReady()),Qt::QueuedConnection);
}

void QDaqDataBuffer::setBackBufferDepth(uint d)
{
	if (d>0 && d!=backBufferDepth())
	{
        os::auto_lock L(comm_lock);

        for(int i=0; i<queue_.size(); i++)
            queue_[i].setCapacity(d);

		emit propertiesChanged();
	}
}

void QDaqDataBuffer::setChannels(QDaqObjectList chlist)
{
	// check the channel list
    foreach(QDaqObject* obj, chlist)
	{
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(obj);
		if (!ch) {
			throwScriptError("Invalid channel object in channel list");
			return;
		}
	}

    os::auto_lock L(comm_lock);

	// clear previous channels
	channel_objects.clear();
	channel_ptrs.clear();
    foreach(const QByteArray& ba, channel_names) setProperty(ba,QVariant());
	channel_names.clear();

	// create channels
	channel_objects = chlist;
	uint cap_ = capacity();
	data_matrix = matrix_t(chlist.size());
    cap_ = backBufferDepth();
    queue_ = matrix_t(chlist.size());
    if (cap_) setBackBufferDepth(cap_); // restore the capacity
    foreach(QDaqObject* obj, chlist)
	{
        channel_ptrs.push_back((QDaqChannel*)obj);
		QByteArray channelName = obj->objectName().toLatin1();
		channel_names.push_back(channelName);
	}
    for(int i=0; i<data_matrix.size(); ++i)
        setProperty(channel_names.at(i),QVariant::fromValue(&(data_matrix[i])));

}

bool QDaqDataBuffer::run()
{

    if (bufferRowsAvailable()==backBufferDepth())
	{
		pushError("Back-buffer full - data lost.");
		//return;
	}

    for(int i=0; i<channel_ptrs.size(); i++)
    {
        channel_t ch = channel_ptrs[i];
        double v(0.);
        if (ch && ch->dataReady()) v = ch->value();
        queue_[i].push(v);
    }

    emit dataReady();

    return QDaqJob::run();
}
void QDaqDataBuffer::onDataReady()
{
    // get real-time data in

    int nread = 0;
    // locked code
    {
        os::auto_lock L(comm_lock);

        nread = bufferRowsAvailable();

        for(int i=0; i<data_matrix.size(); i++)
        {
            vector_t& v = queue_[i];
            for(int j=nread-1; j>=0; j--) data_matrix[i].push(v[j]);
            v.clear();
        }
    }

    if (nread) {
        emit updateWidgets();
        emit propertiesChanged();
    }

}
uint QDaqDataBuffer::bufferRowsAvailable() const
{
    if (queue_.isEmpty()) return 0;
    else return (uint)queue_[0].size();
}
uint QDaqDataBuffer::backBufferDepth() const
{
    if (queue_.isEmpty()) return 0;
    else return (uint)queue_[0].capacity();
}
uint QDaqDataBuffer::size() const
{
	if (data_matrix.isEmpty()) return 0;
	else return (uint)data_matrix[0].size();
}
uint QDaqDataBuffer::columns() const
{
    if (data_matrix.isEmpty()) return 0;
    else return (uint)data_matrix.size();
}
uint QDaqDataBuffer::capacity() const
{
	if (data_matrix.isEmpty()) return 0;
	else return (uint)data_matrix[0].capacity();
}
void QDaqDataBuffer::setCapacity(uint cap)
{
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setCapacity(cap);
}
void QDaqDataBuffer::setType(BufferType t)
{
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setType((vector_t::StorageType)t);
}

