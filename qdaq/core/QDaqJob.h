#ifndef _RTJOB_H_
#define _RTJOB_H_

#include "QDaqObject.h"
#include "math_util.h"

#include <QPointer>

class QDaqScriptEngine;
class QScriptProgram;
class QDaqLoop;

/** Base class for objects that perform a specific task reqursively.

Jobs can be organized as children of an QDaqLoop, which encapsulates a software loop
that executes all child-jobs in each repetition.

In order for the job to perform its task it must also be armed
by calling arm(). Arming does all the necessary initialization in the object.

If the slave property is true, then the job is automatically armed by
its parent job.

\ingroup QDaq-Core
*/
class RTLAB_BASE_EXPORT QDaqJob : public QDaqObject
{
	Q_OBJECT
	/** True if job is armed.
    An armed job is ready to run. If the QDaqJob belongs to a loop
	it will execute its task.
	*/
    Q_PROPERTY(bool armed READ armed)
    /** Script code executed by the job each run.
     */
    Q_PROPERTY(QString code READ code WRITE setCode)

protected:
    // properties
    bool armed_;
    QString code_;
    // bytecode of code
    QScriptProgram* program_;
    // script engine for loop code
    QPointer<QDaqScriptEngine> loop_eng_;

	/** Performs internal initialization for the job.
	*/
    virtual bool arm_();

	/// Disarms the job and performs internal de-initialization.
    virtual void disarm_();

public:
	bool armed() { return armed_; }
    bool setArmed(bool on);

    const QString& code() const { return code_; }
    void setCode(const QString& s);

protected:

	friend class JobList;
    class JobList : public QList<QDaqJob*>
	{
		public:
            bool exec() {
                foreach(QDaqJob* job, *this)
                    if (!job->exec()) return false;
                return true;
            }
            void lock() { for(iterator i=begin(); i<end(); ++i) (*i)->jobLock(); }
            void unlock() { for(iterator i=end()-1; i>=begin(); --i) (*i)->jobUnlock(); }
	};

	friend class JobLocker;
	class JobLocker
	{
        QDaqJob* j;
	public:
        JobLocker(QDaqJob* aj) : j(aj) { j->jobLock(); }
        ~JobLocker()  { j->jobUnlock(); }
	};

    /** A list of jobs that are children of this job.
      The list is populated when the job is armed.
      During task execution, the run() function of all
      jobs in the list is called.
      */
    JobList subjobs_;

    void jobLock() { comm_lock.lock(); subjobs_.lock(); }
    void jobUnlock() { subjobs_.unlock(); comm_lock.unlock(); }

protected:
    virtual bool exec();
    virtual bool run();

	bool throwIfArmed();

public:
    /** Constructor of QDaqJob objects.
    If parent is also a QDaqJob then the currently created QDaqJob is added to the
	list of this parent's child-jobs.
	*/
    Q_INVOKABLE explicit QDaqJob(const QString& name);
    virtual ~QDaqJob(void);

    virtual void attach();
	virtual void detach();

    QDaqLoop* topLoop() const;
    QDaqLoop* loop() const;
    virtual QDaqScriptEngine* loopEngine() const;

public slots:

};

/** Base class of all QDaq loops.

\ingroup QDaq-Core

QDaqLoop encapsulates a loop that is executed reqursively.

If delay cycles is a property of QDaqLoop and can be set by script code.

A real-time timer loop based on win32 timers.

After calling arm() a timer thread is launched which
executes [i.e., calls the exec() function] of the loop
and all its child-jobs. The loop is repetitive
with the period given in ms.

In Windows the timer is realized by means of the timeSetEvent() and
timeKillEvent() functions of the windows SDK.
*/
class RTLAB_BASE_EXPORT QDaqLoop : public QDaqJob
{
    Q_OBJECT
    /** Number of executed loop cycles.
     * It is reset to 0 when the loop is armed.
     */
    Q_PROPERTY(uint count READ count)
    /** Total number of cycles to be executed.
     * If limit is equal to 0 (default) then the loop runs indefinately.
     */
    Q_PROPERTY(uint limit READ limit WRITE setLimit)
    /** Number of parent loop cycles before this loop is called.
    The value of 0 is allowed.
    */
    Q_PROPERTY(uint delay READ delay WRITE setDelay)
    /** Preload the internal counter.
     * Can be used to run loops out of phase
    */
    Q_PROPERTY(uint preload READ preload WRITE setPreload)
    /** The repetition period in ms.
     * This can be set only on the main loop.
    */
    Q_PROPERTY(uint period READ period WRITE setPeriod)

protected:
    uint count_, limit_, delay_, preload_,period_; // properties
    uint delay_counter_;

    bool aborted_;

    virtual bool exec();
    virtual bool arm_();
    virtual void disarm_();
    bool operator()();

    // OS timer thread
    typedef os::timer<QDaqLoop> timer_t;
    friend class os::timer<QDaqLoop>;
    timer_t thread_;

    // Loop performance monitors
    // perfmon[0] : average loop period (ms)
    // perfmon[1] : average loop load-time (ms)
    typedef math::running_average<float,10> perfmon_t;
    perfmon_t perfmon[2];
    os::stopwatch clock_;
    float t_[2];

public:
    Q_INVOKABLE explicit QDaqLoop(const QString& name);
    virtual ~QDaqLoop(void);

    uint limit() const { return limit_; }
    uint count() const { return count_; }
    uint delay() const { return delay_; }
    uint preload() const { return preload_; }
    uint period() const { return period_; }
    void setLimit(uint d);
    void setDelay(uint d);
    void setPreload(uint d);
    void setPeriod(uint p);

    bool isTop() const { return this==topLoop(); }

    QDaqLoop* parentLoop() const;

    virtual QDaqScriptEngine* loopEngine() const;

signals:
    void abort();

public slots:
    /** Arm the loop.
      */
    bool arm() { return setArmed(true); }
    /// Disarm the loop.
    void disarm() { setArmed(false); }
    /** Print loop statistics.
     */
    QString stat();
    void createLoopEngine();
};

#endif


 
