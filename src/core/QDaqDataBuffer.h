#ifndef QDAQDATABUFFER_H
#define QDAQDATABUFFER_H

#include "QDaqVector.h"
#include "QDaqJob.h"

#include <QPointer>
#include <QSemaphore>

class QDaqChannel;

/**
 * @brief A class that provides storage of channel or other data.
 *
 * @ingroup Core
 * @ingroup ScriptAPI
 *
 *
 * If QDaqDataBuffer is used within a QDaqLoop then at each repetition it will store
 * the values of all channels that have been assigned to it. The data are stored
 * internally as a matrix of QDaqBuffer objects, keeping one QDaqBuffer per channel.
 * A new row of data is added at each loop repetition.
 *
 * QDaqDataBuffer has an internal back buffer, where data generated in the loop
 * thread are initially stored. The data is later transferred to the main buffer
 * whenever possible and becomes available to the main application thread.
 * The process is marshalled internally by a semaphore and the Qt signal/slot mechanism.
 * Increasing the size of the back buffer can prevent data loss in fast loops.
 *
 * The QDaqDataBuffer may be also used as a static object outside of a loop.
 * Data may be appended by the push() function.
 *
 */
class QDAQ_EXPORT QDaqDataBuffer : public QDaqJob
{
	Q_OBJECT

    /// Size of the back buffer in rows.
	Q_PROPERTY(uint backBufferDepth READ backBufferDepth WRITE setBackBufferDepth)
    /// Total capacity (allocated memory) of the data buffer in rows.
	Q_PROPERTY(uint capacity READ capacity WRITE setCapacity)
    /// Current size of the data buffer in rows.
	Q_PROPERTY(uint size READ size)
    /// Number of data columns.
    Q_PROPERTY(uint columns READ columns)
    /// True if buffer is circular
    Q_PROPERTY(bool circular READ circular WRITE setCircular)
    /// A QList of the channels monitored by this object.
    Q_PROPERTY(QDaqObjectList channels READ channels WRITE setChannels STORED false)
    /// A list of column names.
    Q_PROPERTY(QStringList columnNames READ columnNames WRITE setColumnNames)

protected:
    // typedefs of channel ptr, channel vector, matrix
    typedef QPointer<QDaqChannel> channel_t;
    typedef QVector<channel_t> channel_vector_t;
    typedef QDaqVector vector_t;
    // data store type = QVector of QDaqBuffer
    typedef QVector<vector_t> matrix_t;

    virtual void writeh5(const QH5Group& h5g, QDaqH5File *f) const;
    virtual void readh5(const QH5Group& h5g, QDaqH5File *f);

protected:
    // properties
    uint backBufferDepth_, capacity_;

    // properties
    bool circular_;
    QDaqObjectList channel_objects;
    channel_vector_t channel_ptrs;
    QStringList columnNames_;

    // back buffer   
    QVector<double> backBuffer_; // memory buffer
    QVector<double*> backPackets_; // packets
    QSemaphore freePackets_, usedPackets_; // used for marshalling the packets
    uint iFree_, iUsed_; // index of free and used packets
    void setupBackBuffer();

	matrix_t data_matrix;

    /**
     * @brief Perform the QDaqDataBuffer tasks within a loop.
     *
     * The following tasks are performed at each loop repetition
     *   - the object tries to acquire a free back buffer packet
     *   - If succesfull it fills the packet with data from the assigned channels
     *     and signals the main thread to collect the packet.
     *   - otherwise it pushes a QDaq error that data got lost
     *
     * @return QDaqJob::run()
     *
     */
    virtual bool run();

    //void resize();

public:
    Q_INVOKABLE explicit QDaqDataBuffer(const QString& name);

    // property getters
    uint backBufferDepth() const { return backBufferDepth_; }
    uint capacity() const { return capacity_; }
	uint size() const;
    uint columns() const;
    bool circular() const { return circular_ ; }
    QDaqObjectList channels() const { return channel_objects; }
    QStringList columnNames() const { return columnNames_; }

    // setters
	void setBackBufferDepth(uint d);
	void setCapacity(uint cap);
    void setCircular(bool on);
    void setChannels(QDaqObjectList chlist);
    void setColumnNames(QStringList collist);

signals:
    // emitted when a data packet becomes available
    void dataReady();

private slots:
    // connected to dataReady. collects all available data packets to the main buffer.
    void onDataReady();

public slots:
    /// Clear all data.
    void clear();
    void addChannels(QDaqObjectList chlist);
    void removeChannels(QDaqObjectList chlist);

    /**
     * @brief Append a new row of values.
     * @param v Vector of data values. v.size() must be equal to size().
     */
    void push(const QDaqVector& v);

    /// Return the i-th QDaqBuffer
    QDaqVector get(int i) { return data_matrix[i]; }
};

Q_DECLARE_METATYPE(QDaqDataBuffer*)

#endif // RTDATABUFFER_H
