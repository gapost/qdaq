#include "QDaqDataBuffer.h"
#include "QDaqChannel.h"

#include <QCoreApplication>
#include <QVariant>

QDaqDataBuffer::QDaqDataBuffer(const QString &name) : QDaqJob(name)
{
    connect(this,SIGNAL(dataReady()),this,SLOT(onDataReady()),Qt::QueuedConnection);

    circular_ = false;
    setBackBufferDepth(2);
    setCapacity(100);

}

void QDaqDataBuffer::setBackBufferDepth(uint d)
{
    if (d>0)
	{
        QMutexLocker L(&comm_lock);

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

void QDaqDataBuffer::removeChannels(QDaqObjectList chlist)
{

    QMutexLocker L(&comm_lock);
    int k;
    foreach(QDaqObject* obj, chlist)
    {
        //check that we remove channel objects indeed
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(obj);
        if (!ch) {
            throwScriptError("Invalid channel object in channel list");
            return;
        }
        //Find the *object name* of the channel destined for deletion
        QString chanName = obj->objectName();
        //Where is it on the columns of the data table
        k = columnNames_.indexOf(chanName);
//        qInfo ("k = %d",k);
        //k=-1 means does not exist in the current channel list
        if (k>=0){
            //remove the channel *object*
            channel_objects.removeOne(obj);
            //set its properties to "empty"
            setProperty(chanName.toLatin1(),QVariant());
            //remove it from column list
            columnNames_.removeAt(k);
            //remove the data column - !!! reminder to save here (or at function entrance?)?
            data_matrix.removeAt(k);
            //remove the channel *pointer*, however smart it is
            channel_ptrs.removeAt(k);
        }
        else{
            throwScriptError("Channel not in current channel list");
            return;
        }
    }

    emit propertiesChanged();

}


void QDaqDataBuffer::addChannels(QDaqObjectList chlist)
{
    // check the new channel list
    foreach(QDaqObject* obj, chlist)
    {
        QDaqChannel* ch = qobject_cast<QDaqChannel*>(obj);
        if (!ch) {
            throwScriptError("Invalid channel object in channel list");
            return;
        }
    }

// add exhaustive checks: names', properties' clashes, If empty previous chlist, create it
// ...

     QMutexLocker L(&comm_lock);

     // append channel objects
    channel_objects.append(chlist);

    uint cap_ = capacity();
    //create the extra data matrix needed
    matrix_t data_matrix_new;
    //it has a size (number of columns) equal to the "new" chlist
    data_matrix_new = matrix_t(chlist.size());
    // create the capacity && type for the new chlist
    for(int i=0; i<data_matrix_new.size(); i++)
    {
        data_matrix_new[i].setCapacity(cap_);
        data_matrix_new[i].setCircular(circular_);
        if(data_matrix.size()){
            //fill the rows with zeros, up to the row size of the *original* data_matrix
            for(int k=0; k<data_matrix[0].size(); k++){
                data_matrix_new[i].push(0);
            }
        }
    }
    //join the two data matrices
    data_matrix.append(data_matrix_new);
    //from now on, work like nothing happened

    setupBackBuffer();

    //append channel pointers vector and column names list
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

    QMutexLocker L(&comm_lock);

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
        data_matrix[i].setCircular(circular_);
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

void QDaqDataBuffer::setColumnNames(QStringList collist)
{
    QMutexLocker L(&comm_lock);

    // clear previous channels & columns
    channel_objects.clear();
    channel_ptrs.clear();
    foreach(const QString& str, columnNames_)
        setProperty(str.toLatin1(),QVariant());
    columnNames_.clear();

    // create channels
    columnNames_ = collist;

    uint cap_ = capacity();
    data_matrix = matrix_t(collist.size());
    // restore the capacity && type
    for(int i=0; i<data_matrix.size(); i++)
    {
        data_matrix[i].setCapacity(cap_);
        data_matrix[i].setCircular(circular_);
    }

    setupBackBuffer();

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
        QMutexLocker L(&comm_lock);

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
    if (cap==capacity()) return;

    if (cap>0) {
        QMutexLocker L(&comm_lock);
        for(int i=0; i<data_matrix.size(); i++)
            data_matrix[i].setCapacity(cap);
        capacity_ = cap;
        emit propertiesChanged();
    }
}
void QDaqDataBuffer::setCircular(bool on)
{
    if (on==circular_) return;
    QMutexLocker L(&comm_lock);
	for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].setCircular(on);
    circular_ = on;
    emit propertiesChanged();
}
void QDaqDataBuffer::clear()
{
    QMutexLocker L(&comm_lock);
    for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].clear();
    emit propertiesChanged();
    emit updateWidgets();
}
void QDaqDataBuffer::push(const QDaqVector &v)
{
    QMutexLocker L(&comm_lock);

    if (v.size()!=data_matrix.size()) return;

    for(int j=0; j<data_matrix.size(); j++)
        data_matrix[j].push(v[j]);

    uint c = data_matrix[0].capacity();
    if (c!=capacity_) capacity_ = c;

    emit updateWidgets();
    emit propertiesChanged();
}


