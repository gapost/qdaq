#ifndef QTIMERTHREAD_H
#define QTIMERTHREAD_H


#include <QThread>
#include <QTimer>

/**
 * @brief A timer thread class.
 *
 * QTimerThread inheritis QThread and uses internally QTimer to implement
 * its functionality.
 *
 * After calling start() a thread is spawned and the function timer_func()
 * is called periodically with period equal to the interval property (in ms).
 *
 * To use the class one has to subclass QTimerThread reimplement timer_func(). Note
 * that timer_func() is executed from the sepatrate timer thread thus and the user
 * is responsible for synchronization issues.
 *
 * The accuracy of the timer depends on Qt's QTimer class. According to their docs
 * QTimer uses the best OS timing routines in each case.
 *
 * @ingroup Core
 */

class QTimerThread : public QThread
{
    Q_OBJECT

    /** Timer repetition interval in ms. */
    Q_PROPERTY(int interval READ interval WRITE setInterval)

    QTimer timer_;

protected slots:
    virtual void timer_func() {}

protected:
    virtual void run();

public:
    QTimerThread();

    virtual ~QTimerThread();

    int interval() const { return timer_.interval(); }
    void setInterval(int ms) { timer_.setInterval(ms); }

};

#endif // QTIMERTHREAD_H
