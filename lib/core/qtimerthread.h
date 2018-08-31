#ifndef QTIMERTHREAD_H
#define QTIMERTHREAD_H


#include <QThread>
#include <QTimer>
#include <QElapsedTimer>


/** timer thread
  * A timer thread implemented using timer_create() + signal
  * + a thread that watches for the signal
  */

class QTimerThread : public QThread
{
    Q_OBJECT

    QTimer timer_;
    QElapsedTimer clock_;

protected slots:
    virtual void timer_func() {}

protected:
    virtual void run();

public:
    QTimerThread();

    virtual ~QTimerThread();

    int interval() const { return timer_.interval(); }
    void setInterval(int ms) { timer_.setInterval(ms); }

    quint64 elapsed() const { return clock_.elapsed(); }
    quint64 nsecsElapsed() const { return clock_.nsecsElapsed(); }



};

#endif // QTIMERTHREAD_H
