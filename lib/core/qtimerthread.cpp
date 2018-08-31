#include "qtimerthread.h"

QTimerThread::QTimerThread()
{
    timer_.setInterval(1000);
    timer_.setTimerType(Qt::PreciseTimer);
    timer_.moveToThread(this);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(timer_func()), Qt::DirectConnection);
}

QTimerThread::~QTimerThread()
{
    quit();
    wait();
}

void QTimerThread::run()
{

    timer_.start();
    clock_.start();
    QThread::run();
    timer_.stop();
}
