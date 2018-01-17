#include "RtDataBuffer.h"
#include "RtDataChannel.h"

#include <QCoreApplication>
#include <QVariant>

#include "RtEnumHelper.h"
Q_SCRIPT_ENUM(BufferType,RtDataBuffer)

void RtDataBuffer::registerTypes(QScriptEngine* e)
{
	qScriptRegisterBufferType(e);
	RtJob::registerTypes(e);
}

RtDataBuffer::RtDataBuffer(const QString &name) : RtJob(name),
	type_(Open)
{
	queue_.alloc(4,1);
}

void RtDataBuffer::setBackBufferDepth(uint d)
{
	if (d>0 && d!=backBufferDepth())
	{
		auto_lock L(comm_lock);

		queue_.alloc(d, queue_.width());

		emit propertiesChanged();
	}
}

void RtDataBuffer::setChannels(RtObjectList chlist)
{
	// check the channel list
	foreach(RtObject* obj, chlist)
	{
		RtDataChannel* ch = qobject_cast<RtDataChannel*>(obj);
		if (!ch) {
			throwScriptError("Invalid channel object in channel list");
			return;
		}
	}

	auto_lock L(comm_lock);

	// clear previous channels
	channel_objects.clear();
	channel_ptrs.clear();
	foreach(const QByteArray& ba, channel_names) setProperty(ba,QVariant());
	channel_names.clear();

	// create channels
	channel_objects = chlist;
	uint cap_ = capacity();
	data_matrix = matrix_t(chlist.size());
	if (cap_) setCapacity(cap_); // restore the capacity
	foreach(RtObject* obj, chlist)
	{
		channel_ptrs.push_back((RtDataChannel*)obj);
		QByteArray channelName = obj->objectName().toLatin1();
		channel_names.push_back(channelName);
	}
	for(int i=0; i<data_matrix.size(); ++i)
		setProperty(channel_names.at(i),QVariant::fromValue(&(data_matrix[i])));

}

void RtDataBuffer::run()
{
	// lock the back-buffer
	queue_.access_lock.lock();

	if (queue_.full())
	{
		pushError("Back-buffer full - data lost.");
		//return;
	}

	double* p = queue_.getWriteBuffer();
	double* q = p;

	foreach(channel_t ch, channel_ptrs)
	{
		double v(0);
		if (ch && ch->dataReady())
		{
			v = ch->value();
		}
		*q++ = v;
	}

	queue_.releaseWriteBuffer();

	// unlock the back-buffer
	queue_.access_lock.unlock();

	QCoreApplication::postEvent(this, new RtEvent(RtEvent::DataQueueDataReady,this));

	RtJob::run();
}
void RtDataBuffer::rtEvent(RtEvent* e)
{
	if (e->rtType()==RtEvent::DataQueueDataReady)
	{
		// get real-time data in

		int nread = 0;

		// lock the back-buffer
		queue_.access_lock.lock();

		while(queue_.count())
		{
			const double* p = queue_.getReadBuffer();

			for(int i=0; i<data_matrix.size(); i++)
				data_matrix[i].push(*p++);

			queue_.releaseReadBuffer();
			nread++;
		}

		queue_.access_lock.unlock();

		if (nread) updateWidgets();

	}
	else RtJob::rtEvent(e);
}
uint RtDataBuffer::size() const
{
	if (data_matrix.isEmpty()) return 0;
	else return (uint)data_matrix[0].size();
}
uint RtDataBuffer::capacity() const
{
	if (data_matrix.isEmpty()) return 0;
	else return (uint)data_matrix[0].capacity();
}
void RtDataBuffer::setCapacity(uint cap)
{
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setCapacity(cap);
}
void RtDataBuffer::setType(BufferType t)
{
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setType((vector_t::StorageType)t);
}

