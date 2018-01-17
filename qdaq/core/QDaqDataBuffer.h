#ifndef RTDATABUFFER_H
#define RTDATABUFFER_H

#include "RtJob.h"

#include "RtTypes.h"

#include "DataQueue.h"

#include <QPointer>

//#include <QStringList>

class RtDataChannel;

class RTLAB_BASE_EXPORT RtDataBuffer : public RtJob
{
	Q_OBJECT

	Q_PROPERTY(uint backBufferDepth READ backBufferDepth WRITE setBackBufferDepth)

	Q_PROPERTY(uint capacity READ capacity WRITE setCapacity)
	Q_PROPERTY(uint size READ size)
	Q_PROPERTY(BufferType type READ type WRITE setType)
	Q_PROPERTY(RtObjectList channels READ channels WRITE setChannels)

	Q_ENUMS(BufferType)

protected:
	typedef RtDoubleBuffer vector_t;

public:
	enum BufferType {
		Open = vector_t::Open,
		Fixed = vector_t::Fixed,
		Circular = vector_t::Circular
	};

	virtual void registerTypes(QScriptEngine *e);

protected:
	typedef QPointer<RtDataChannel> channel_t;
	typedef QList<channel_t> channel_list;
	typedef QVector<vector_t> matrix_t;

	BufferType type_;

	DataQueue<double> queue_;

	RtObjectList channel_objects;
	channel_list channel_ptrs;
	QList<QByteArray> channel_names;

	matrix_t data_matrix;

	virtual void run();

	virtual void rtEvent(RtEvent* e);

	void resize();

public:
	Q_INVOKABLE
	RtDataBuffer(const QString& name);

	uint backBufferDepth() const { return (uint)queue_.depth(); }
	uint capacity() const;
	uint size() const;
	BufferType type() const { return type_ ; }
	RtObjectList channels() const { return channel_objects; }

	void setBackBufferDepth(uint d);
	void setCapacity(uint cap);
	void setType(BufferType t);
	void setChannels(RtObjectList chlist);

};



#endif // RTDATABUFFER_H
