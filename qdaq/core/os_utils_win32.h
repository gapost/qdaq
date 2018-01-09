#ifndef OS_UTILS_WIN32_H
#define OS_UTILS_WIN32_H

#include <sys/timeb.h>
#define WIN32_LEAN_AND_MEAN
#include <qt_windows.h>
//#include <windows.h>
#include <mmsystem.h>

namespace os {

inline void beep()
{
    MessageBeep(0xFFFFFFFF);
}

// a win32 critical section

/**
 * @brief A critical section for thread syncronization
 *
 */
class critical_section
{
public:
        critical_section()  { InitializeCriticalSection(&section); }
        ~critical_section() { DeleteCriticalSection(&section); }
        void lock()         { EnterCriticalSection(&section); }
        void unlock()       { LeaveCriticalSection(&section); }
        operator CRITICAL_SECTION& () { return section; }

private:
        CRITICAL_SECTION section;
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

/**
 * @brief A class encapsulating a Win32 wait condition.
 *
 *
 */
class wait_condition
{
    CONDITION_VARIABLE cond;
public:
    wait_condition()
    {
        InitializeConditionVariable(&cond);
    }
    ~wait_condition()
    {
        // ??
    }
    void signal()
    {
        WakeConditionVariable(&cond);
    }
    int wait(critical_section& cs)
    {
        CRITICAL_SECTION& mycs = cs;
        return SleepConditionVariableCS(&cond,&mycs,INFINITE);
    }
};

/**
 * @brief A high resolution stop-watch
 *
 * In Win32 implemented with the QueryPerformanceCounter function.
 *
 */
class stopwatch
{
private:
    typedef __int64 i64;

    bool running_;
    i64 start_time_;
    i64 t_;
    i64 total_;
    i64 counts_per_sec_;

    const i64& latch()
    {
        static i64 t;
        QueryPerformanceCounter((LARGE_INTEGER*)&t_);
        return t_;
    }

public:
    stopwatch() :  running_(false), start_time_(0), total_(0)
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&counts_per_sec_);
    }

    /**
        Start timing from 0.00.
    */
    void start()
    {
        start_time_ = latch();
        total_ = 0;
        running_ = true;
    }

    /**
        Stop timing
    */
    void stop()
    {
        if (running_)
        {
             total_ += (latch() - start_time_);
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
            start_time_ = latch();
            running_ = true;
        }
    }

    void reset() { total_ = 0; }

    /**
        Read current time (in seconds).
    */
    double sec()
    {
        return 1.*ticks()/counts_per_sec_;
    }

    /**
        Read current time (in base frequency periods).
    */
    const i64& ticks()
    {
        if (running_)
        {
            stop();
            resume();
        }
        return total_;
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
    HANDLE  tid;
    Functor* F;

    volatile int running_;
    critical_section cs;
    wait_condition cond;

    int thread_func_()
    {
        cs.lock();
        running_ = 1;
        cond.signal();
        cs.unlock();

        (*F)();

        running_ = 0;

        return 0;
    }

    static DWORD WINAPI thread_func(LPVOID arg)
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
        cs.lock();

        // Create the thread
        DWORD threadid;
        tid = CreateThread(NULL,
                    0,
                    thread_func,
                    this,
                    0,
                    &threadid);
        if (tid==NULL)
        {
            cs.unlock();
            return false;
        }

        // wait for thread to start
        cond.wait(cs);
        cs.unlock();

        return true;
    }
    int is_running() const
    {
        return running_;
    }
    void wait()
    {
        if (!running_) return;
        HANDLE h = (HANDLE)InterlockedExchange((LONG*)&tid, 0);
        if (h)
        {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
    operator HANDLE () const { return tid; }
};

/** timer thread
  * A timer thread implemented using timeSetEvent() function
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

    int timerId;
    unsigned long long wakeups_missed;
    unsigned int period; // ms
    myFunctor myF;
    Functor* F;
    critical_section cs;
    wait_condition cond;
    volatile unsigned ticks;


    volatile int continue_;

    // windows timer callback
    static void CALLBACK _timerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
    {
        wTimerID = wTimerID;
        msg = msg;
        dw1 = dw1;
        dw2 = dw2;
        self_t* t = reinterpret_cast<self_t*>(dwUser);
        if (t) t->timer_signal();
    }

    // Arm/disarm the timer
    int arm (unsigned ms)
    {
        if (ms)
        {
            timerId = timeSetEvent(ms,0,_timerProc,(DWORD)this,
                TIME_CALLBACK_FUNCTION | TIME_PERIODIC); // TIME_KILL_SYNCHRONOUS only winXP

        } else {
            if (timerId)
            {
                timeKillEvent(timerId);
                timerId = 0; // wait for thread to die
            }
        }
        return timerId;
    }

    int timer_signal()
    {
        cs.lock();
        ticks++;
        cond.signal();
        cs.unlock();
        return 0;
    }

    int wait_period ()
    {
        cs.lock();
        if (ticks) wakeups_missed += ticks; // the clock has already ticked
        cond.wait(cs);
        ticks = 0;
        cs.unlock();
        return 0;
    }

    void timer_func()
    {
        arm(period);
        wait_period();
        while (continue_)
        {
            F->operator()();
            wait_period();
        }
        arm(0);
    }

public:
    timer() : timerId(0)
    {
        timeBeginPeriod(1U);
    }
    virtual ~timer()
    {
        stop();
        timeEndPeriod(1U);
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

/*
template<class Functor>
class timer
{
private:
    unsigned int timerId;

    // windows timer callback
    static void CALLBACK _timerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
    {
        wTimerID = wTimerID;
        msg = msg;
        dw1 = dw1;
        dw2 = dw2;
        Functor* f = reinterpret_cast<Functor*>(dwUser);
        if (f) (*f)();
    }

public:

    timer() : timerId(0)
    {
        timeBeginPeriod(1U);
    }
    virtual ~timer()
    {
        stop();
        timeEndPeriod(1U);
    }
    bool start(Functor* f, unsigned int ms)
    {
        stop();
        timerId = timeSetEvent(ms,0,_timerProc,(DWORD)f,
            TIME_CALLBACK_FUNCTION | TIME_PERIODIC); // TIME_KILL_SYNCHRONOUS only winXP
        return timerId != 0;
    }
    void stop()
    {
        if (timerId)
        {
            timeKillEvent(timerId);
            timerId = 0; // wait for thread to die
        }
    }
    bool is_running() const { return timerId != 0; }
};
*/
} // namespace os


#endif // WIN32UTILS_H
