#ifndef OS_UTILS_LINUX_H
#define OS_UTILS_LINUX_H

#include <time.h>
#include <pthread.h>
#include <sys/timerfd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

namespace os {

static inline void beep()
{
    printf("\a");
}

class critical_section
{
    pthread_mutex_t cs_mutex;
public:
    critical_section()
    {
        pthread_mutexattr_t a;
        pthread_mutexattr_init(&a);
        pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init( &cs_mutex, &a);

        pthread_mutexattr_destroy(&a);
    }
    ~critical_section()
    {
        pthread_mutex_destroy(&cs_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&cs_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&cs_mutex);
    }
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class auto_lock
{
public:
    auto_lock(critical_section& cs) : cs_(cs) { cs_.lock(); }
    ~auto_lock() { cs_.unlock(); };

private:
    critical_section& cs_;
};

class mutex
{
    pthread_mutex_t cs_mutex;
public:
    mutex()
    {
        pthread_mutexattr_t a;
        pthread_mutexattr_init(&a);
        pthread_mutexattr_settype(&a, PTHREAD_MUTEX_NORMAL);

        pthread_mutex_init( &cs_mutex, &a);

        pthread_mutexattr_destroy(&a);
    }
    ~mutex()
    {
        pthread_mutex_destroy(&cs_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&cs_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&cs_mutex);
    }
    pthread_mutex_t& handle()
    {
        return cs_mutex;
    }
};

// locks a mutex, and unlocks it automatically
// when the lock goes out of scope
class mutex_lock
{
public:
    mutex_lock(mutex& cs) : cs_(cs) { cs_.lock(); }
    ~mutex_lock() { cs_.unlock(); };

private:
    mutex& cs_;
};

class wait_condition
{
    pthread_cond_t cond;
public:
    wait_condition()
    {
        pthread_cond_init(&cond,NULL);
    }
    ~wait_condition()
    {
        pthread_cond_destroy(&cond);
    }
    int signal()
    {
        return pthread_cond_signal(&cond);
    }
    int wait(mutex& mtx)
    {
        return pthread_cond_wait(&cond,&(mtx.handle()));
    }
};

#define SW_CLOCK_ID CLOCK_MONOTONIC

class stopwatch
{
    bool running_;
    timespec start_time_;
    timespec resol_;
    timespec total_;

    int latch(timespec& t_)
    {
        return clock_gettime(SW_CLOCK_ID,&t_);
    }

public:
    stopwatch() :  running_(false)
    {
        clock_getres(SW_CLOCK_ID,&resol_);
    }

    /**
        Start timing from 0.00.
    */
    void start()
    {
        reset();
        latch(start_time_);
        running_ = true;
    }

    /**
        Stop timing
    */
    void stop()
    {
        if (running_)
        {
            timespec t;
            latch(t);
            total_.tv_sec += t.tv_sec - start_time_.tv_sec;
            total_.tv_nsec += t.tv_nsec - start_time_.tv_nsec;
            running_ = false;
        }
    }

    /**
        Resume timing, if currently stopped.  Operation
        has no effect if Stopwatch is already running_.
    */
    void resume()
    {
        if (!running_)
        {
            latch(start_time_);
            running_ = true;
        }
    }

    void reset() { total_.tv_sec = total_.tv_nsec = 0; }

    /**
        Read current time (in seconds).
    */
    double sec()
    {
        if (running_)
        {
            stop();
            resume();
        }
        return 1.e-9*total_.tv_nsec + total_.tv_sec;
    }


    /**
        Check if clock is running.
    */
    bool is_running() const
    {
        return running_;
    }
};




template < class Functor >
class thread
{
    pthread_t  tid;
    Functor* F;

    volatile int running_;
    mutex mtx;
    wait_condition cond;

    void* thread_func_()
    {
        mtx.lock();
        running_ = 1;
        cond.signal();
        mtx.unlock();

        (*F)();

        running_ = 0;

        return NULL;
    }

    static void *thread_func(void *arg)
    {
        return ((thread*)arg)->thread_func_();
    }

public:
    thread() : tid(0), F(0), running_(0)
    {

    }
     bool start(Functor* f)
    {
        if (running_) return false;

        //store the func
        F = f;

        // lock mutex
        mtx.lock();

        // Create the timer thread
        if (pthread_create(&tid, (pthread_attr_t *)NULL, thread_func, this)!=0)
        {
            mtx.unlock();
            return false;
        }

        // wait for thread to start
        cond.wait(mtx);
        mtx.unlock();

        return true;
    }
    bool is_running() const
    {
        return running_;
    }
    void wait()
    {
        if (!running_) return;

        void* ret;
        pthread_join(tid,&ret);
    }
};



/** timer thread
  * A timer thread implemented using timer_create() + signal
  * + a thread that watches for the signal
  */
template<class Functor>
class timer
{
    typedef timer<Functor> self_t;

    struct myFunctor
    {
        timer* t;
        void operator()()
        {
            t->timer_func();
        }
    };

    thread<myFunctor> thread_;

    int timer_fd;
    unsigned long long wakeups_missed;
    unsigned int period; // ms
    myFunctor myF;
    Functor* F;

    volatile int continue_;

    // Arm/disarm the timer
    int arm (unsigned ms)
    {
        unsigned int ns;
        unsigned int sec;
        itimerspec itval;

        if (ms) {
            sec = period/1000;
            ns = (period - (sec * 1000)) * 1000000;
        }
        else sec=ns=0;
        itval.it_interval.tv_sec = sec;
        itval.it_interval.tv_nsec = ns;
        itval.it_value.tv_sec = sec;
        itval.it_value.tv_nsec = ns;
        return timerfd_settime (timer_fd, 0, &itval, NULL);
    }

    int wait_period ()
    {
        unsigned long long missed;
        int ret;

        /* Wait for the next timer event. If we have missed any the
           number is written to "missed" */
        ret = read (timer_fd, &missed, sizeof (missed));
        if (ret == -1)
        {
            //perror ("read timer");
            return ret;
        }

        wakeups_missed += missed;

        return ret;
    }

    void timer_func()
    {
        arm(period);
        wait_period();
        while (continue_ && F->operator()()) wait_period();
        arm(0);
    }

public:
    timer()
    {
        timer_fd = timerfd_create(CLOCK_MONOTONIC,0);
    }
    ~timer()
    {
        stop();
        if (timer_fd!=-1) close(timer_fd);
    }
    bool start(Functor* f, unsigned int ms)
    {
        stop();

        // copy options
        F = f;
        period = ms;
        myF.t = this;
        continue_ = true;

        return thread_.start(&myF);
    }
    bool is_running() const
    {
        return thread_.is_running();
    }
    void stop()
    {
        if (!thread_.is_running()) return;

        continue_ = false;
        thread_.wait();
    }
};


} // namespace os



#endif // OS_UTILS_LINUX_H
