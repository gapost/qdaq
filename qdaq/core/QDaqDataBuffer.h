#ifndef QDAQDATABUFFER_H
#define QDAQDATABUFFER_H

#include "QDaqTypes.h"
#include "QDaqJob.h"

#include <QPointer>
#include <QSemaphore>

class QDaqChannel;

class RTLAB_BASE_EXPORT QDaqDataBuffer : public QDaqJob
{
	Q_OBJECT

	Q_PROPERTY(uint backBufferDepth READ backBufferDepth WRITE setBackBufferDepth)
	Q_PROPERTY(uint capacity READ capacity WRITE setCapacity)
	Q_PROPERTY(uint size READ size)
    Q_PROPERTY(uint columns READ columns)
	Q_PROPERTY(BufferType type READ type WRITE setType)
    Q_PROPERTY(QDaqObjectList channels READ channels WRITE setChannels STORED false)
    Q_PROPERTY(QStringList columnNames READ columnNames)

	Q_ENUMS(BufferType)

protected:
    typedef QDaqBuffer vector_t;

public:
	enum BufferType {
		Open = vector_t::Open,
		Fixed = vector_t::Fixed,
		Circular = vector_t::Circular
	};

	virtual void registerTypes(QScriptEngine *e);
    virtual void writeH5(H5::Group* h5g) const;
    virtual void readH5(H5::Group *h5g);

protected:
    uint backBufferDepth_, capacity_;
    typedef QPointer<QDaqChannel> channel_t;
    typedef QVector<channel_t> channel_vector_t;
	typedef QVector<vector_t> matrix_t;

	BufferType type_;

    QDaqObjectList channel_objects;
    channel_vector_t channel_ptrs;
    QStringList columnNames_;

    // back buffer
    QVector<double> backBuffer_;
    QVector<double*> backPackets_;
    QSemaphore freePackets_, usedPackets_;
    uint iFree_, iUsed_;
    void setupBackBuffer();

	matrix_t data_matrix;

    virtual bool run();

	void resize();

public:
    Q_INVOKABLE explicit QDaqDataBuffer(const QString& name);

    uint backBufferDepth() const { return backBufferDepth_; }
    uint capacity() const { return capacity_; }
	uint size() const;
    uint columns() const;
	BufferType type() const { return type_ ; }
    QDaqObjectList channels() const { return channel_objects; }
    QStringList columnNames() const { return columnNames_; }

	void setBackBufferDepth(uint d);
	void setCapacity(uint cap);
	void setType(BufferType t);
    void setChannels(QDaqObjectList chlist);

signals:
    void dataReady();

private slots:
    void onDataReady();

};



#endif // RTDATABUFFER_H
