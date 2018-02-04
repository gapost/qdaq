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

QDaqDataBuffer::QDaqDataBuffer(const QString &name) : QDaqJob(name)
{
    connect(this,SIGNAL(dataReady()),this,SLOT(onDataReady()),Qt::QueuedConnection);

    type_ = Fixed;
    setBackBufferDepth(2);
    setCapacity(100);

}

void QDaqDataBuffer::setBackBufferDepth(uint d)
{
    if (d>0)
	{
        os::auto_lock L(comm_lock);

        // depth should be power of 2
        uint n = 1;
        while (n<d) n <<= 1;
        backBufferDepth_ = n;

        setupBackBuffer();

		emit propertiesChanged();
	}
}

void QDaqDataBuffer::setupBackBuffer()
{
    // allocate memory
    uint cols = columns();
    uint N = backBufferDepth_*cols;
    backBuffer_.resize(N);

    // setup packet pointers
    backPackets_.resize(backBufferDepth_);
    double* p = backBuffer_.data();
    for(uint i=0; i<backBufferDepth_; i++)
    {
        backPackets_[i] = p;
        p += cols;
    }

    // setup semaphores
    if (freePackets_.available())
        freePackets_.acquire(freePackets_.available());
    if (usedPackets_.available())
        usedPackets_.acquire(usedPackets_.available());
    freePackets_.release(backBufferDepth_);

    // setup indexes
    iFree_ = iUsed_ = 0;

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
    foreach(const QString& str, columnNames_)
        setProperty(str.toLatin1(),QVariant());
    columnNames_.clear();

	// create channels
	channel_objects = chlist;

	uint cap_ = capacity();
	data_matrix = matrix_t(chlist.size());
    // restore the capacity && type
    for(int i=0; i<data_matrix.size(); i++)
    {
        data_matrix[i].setCapacity(cap_);
        data_matrix[i].setType((vector_t::StorageType)type_);
    }

    setupBackBuffer();

    foreach(QDaqObject* obj, chlist)
	{
        channel_ptrs.push_back((QDaqChannel*)obj);
        columnNames_.push_back(obj->objectName());
	}

    for(int i=0; i<data_matrix.size(); ++i) {
        QString str = columnNames_.at(i);
        QVariant v = QVariant::fromValue(data_matrix[i]);
        setProperty(str.toLatin1(),v);
    }

    emit propertiesChanged();

}

bool QDaqDataBuffer::run()
{


    if (freePackets_.tryAcquire())
    {
        double* p = backPackets_[iFree_ % backBufferDepth_];
        iFree_++;


        for(int i=0; i<channel_ptrs.size(); i++)
        {
            channel_t ch = channel_ptrs[i];
            *p = 0.;
            if (ch && ch->dataReady()) *p = ch->value();
            p++;
        }

        usedPackets_.release();
        emit dataReady();

    }
    else
	{

		pushError("Back-buffer full - data lost.");
		//return;
	}

    return QDaqJob::run();
}
void QDaqDataBuffer::onDataReady()
{
    // get real-time data in

    int nread = 0;
    // locked code
    {
        os::auto_lock L(comm_lock);

        nread = usedPackets_.available();

        if (nread) {

            usedPackets_.acquire(nread);

            for(int i=0; i<nread; i++)
            {
                double* p = backPackets_[iUsed_ % backBufferDepth_];
                iUsed_++;
                for(int j=0; j<data_matrix.size(); j++)
                    data_matrix[j].push(*p++);
            }

            freePackets_.release(nread);
        }
    }

    if (nread) {
        uint c = data_matrix[0].capacity();
        if (c!=capacity_) capacity_ = c;
        emit updateWidgets();
        emit propertiesChanged();
    }

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
void QDaqDataBuffer::setCapacity(uint cap)
{
    if (cap>0) {
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setCapacity(cap);
    capacity_ = cap;
    emit propertiesChanged();
    }
}
void QDaqDataBuffer::setType(BufferType t)
{
	for(int i=0; i<data_matrix.size(); i++)
		data_matrix[i].setType((vector_t::StorageType)t);
    type_ = t;
    emit propertiesChanged();
}
void QDaqDataBuffer::clear()
{
    for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].clear();
    emit propertiesChanged();
    emit updateWidgets();

}

