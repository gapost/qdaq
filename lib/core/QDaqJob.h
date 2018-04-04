#ifndef _RTJOB_H_
#define _RTJOB_H_

#include "QDaqObject.h"
#include "math_util.h"

#include <QPointer>

class QDaqScriptEngine;
class QScriptProgram;
class QDaqLoop;

/** Base class for objects that perform a specific task reqursively.
 *
 * @ingroup Core
 * @ingroup ScriptAPI
 *
Jobs can be organized as children of a parent-job or children of
the super-class QDaqLoop.

When a QDaqLoop runs all its child jobs are executed by calling their exec() function.
The exec() function first calls run() to perform the actual processing of this job
(if there is any) and
then calls exec() of each child job.

To give an example of the order that jobs are executed, the following job tree:
\verbatim
job0
 |--job1
 |   |--job11
 |   |--job12
 |--job2
 |--job3
 \endverbatim
 will be executed with the following order:
 job0-job1-job11-job12-job2-job3

Before the job can perform its task it must be "armed".
Arming does all the necessary initialization and is implemented in the function
setArmed().

*/
class QDAQ_EXPORT QDaqJob : public QDaqObject
{
	Q_OBJECT

	/** True if job is armed.
     *
     * An armed job is ready to run. If the armed QDaqJob belongs to a loop
     * it will execute its task.
     *
     * This property is read-only.
     *
     */
    Q_PROPERTY(bool armed READ armed)
    /** Script code executed by the job at each repetition.
     *
     * If code is empty then nothing will be executed.
     *
     * The script code is executed by the top loop script engine,
     * which is obtained by loopEngine().
     *
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
     *
     * It is called by the setArmed() function.
     *
     * This function can be reimplemented to perform specific initialization
     * for a job. The parent class arm_() function should be called
     * afterwards.
     *
     * In the default implementation, if a loopEngine() exists
     *  the script code is checked against the script engine for errors.
     *
     * If initialization is sucessfull it returns true, otherwise the function
     * returns false.
     *
     */
    virtual bool arm_();

    /** Performs internal de-initialization.
     *
     * It is called by the setArmed() function.
     *
     * Can be reimplemented to define special behavior during dis-arming.
     * The parent class disarm_() should be called.
     *
     */
    virtual void disarm_();

public:
	bool armed() { return armed_; }

    /**
     * @brief Arms or disarms a job
     *
     * If on is true then setArmed() is called for each child-job and
     * finally arm_() is called to initialize this job. If any of those calls
     * return false the arming is aborted and false is returned.
     *
     * If on is false then disarm_() is called first and then setArmed() with parameter
     * false is
     * called for each child job.
     *
     * @param on If true the job is armed else it is disarmed.
     * @return true if the operation was sucesfull.
     */
    bool setArmed(bool on);

    const QString& code() const { return code_; }
    void setCode(const QString& s);

protected:

    // a helper class to manage child-jobs
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
    JobList subjobs_;

    // lock my child jobs
    void jobLock() { comm_lock.lock(); subjobs_.lock(); }
    // unlock my child jobs
    void jobUnlock() { subjobs_.unlock(); comm_lock.unlock(); }

protected:
    /**
     * @brief Called when the job is executed.
     * If armed is false, the function does nothing.
     *
     * If armed is true, then this job's run() is called
     * and then the exec() of all sub-jobs.
     *
     * The function is called by the parent loop's
     * QDaqLoop::exec() function.
     *
     * @return false if run() return false or some child-job returns false; true otherwise.
     */
    virtual bool exec();

    /**
     * @brief This function performs the actual task of this QDaqJob.
     *
     * It can be reimplemented in subclasses to define
     * the specific job that must be performed.
     *
     * In the base implementation, it  executes the script code
     * if available.
     *
     * @return false if a serious error occured, true otherwise.
     */
    virtual bool run();

    // Throws script exception and error if called while the job is armed.
	bool throwIfArmed();

public:
    Q_INVOKABLE
    /**
     * @brief QDaqJob constructor.
     * @param name The QDaqObject name.
     */
    explicit QDaqJob(const QString& name);
    virtual ~QDaqJob(void);

    virtual void attach();
	virtual void detach();

    /// Returns the top level loop that this job belongs to.
    QDaqLoop* topLoop() const;
    /// Returns the loop that this job belongs to.
    QDaqLoop* loop() const;
    /// Returns the QDaqScriptEngine of the top level loop.
    virtual QDaqScriptEngine* loopEngine() const;
};

/**
 * @brief A class encapsulating a software loop.
 *
 * @ingroup Core
 * @ingroup ScriptAPI

QDaqLoop is used to execute a number of child-jobs or child-loops.

By setting the class properties we can arrange for a number of different
execution scenarios:
  * repetitive with given period
  * repeat for ever or for a given number of cycles
  * skip execution every delay cycles
  * single shot

A QDaqLoop that has no QDaqLoop ancestor is considered a "top level loop"
(isTop() returns true). Otherwise the loop is a child-loop.

When arm() is called on a top level loop, a new timer thread is spawned that
calls exec() at each timer repetition.

The timer thread uses facilities of the operating system (see os::timer class).

If arm() is called on a child loop, then it simply arms all child jobs.
The exec() function of a child loop is called from the top level loop thread.

To use QDaqLoop, create the object and build underneath it
a tree of child-jobs and child-loops that
perform the required tasks in the correct order.
Then attach the loop to the QDaq tree either as a child of the root object ("qdaq")
or under another object that does not have a QDaqLoop anchestor. Thus the loop becomes top
level.

 */
class QDAQ_EXPORT QDaqLoop : public QDaqJob
{
    Q_OBJECT

    /** Number of executed loop cycles (read-only).
     * It is reset to 0 when the loop is armed.
     */
    Q_PROPERTY(uint count READ count)

    /** Total number of cycles to be executed.
     * If limit is equal to 0 (default) then the loop runs indefinately.
     */
    Q_PROPERTY(uint limit READ limit WRITE setLimit)

    /** Number of parent loop cycles before this loop is called.
    *
    * The loop is executed every delay repetitions of the parent loop.
    *
    * If delay is 0 or 1 then the loop is executed at each repetition.
    */
    Q_PROPERTY(uint delay READ delay WRITE setDelay)

    /** Preload the internal counter.
     *
     * Can be used in combination with delay to run loops out of phase.
     *
     */
    Q_PROPERTY(uint preload READ preload WRITE setPreload)

    /** The repetition period in ms.
     * This is meaningful only for the top level loop.
     */
    Q_PROPERTY(uint period READ period WRITE setPeriod)

protected:
    uint count_, limit_, delay_, preload_,period_; // properties
    uint delay_counter_;
    bool aborted_;

    /**
     * @brief Called when a loop is executed.
     *
     * In a top level loop this function is called
     * at each repetition of the timer thread.
     *
     * In a child-loop exec() is called from the parent loop
     * according to the order of the child-loop in the
     * tree structure.
     *
     * This function first locks the mutexes of all child jobs
     * to prevent access from other threads. Then it calls
     * QDaqJob::exec() which runs all child jobs. Finally it unlocks
     * the mutexes in the reverse order as they were locked.
     *
     * The signals updateWidgets() and propertiesChanged()
     * are emitted at each valid repetition.
     *
     * @return
     */
    virtual bool exec();
    virtual bool arm_();
    virtual void disarm_();

    // OS timer thread
    typedef os::timer<QDaqLoop> timer_t;
    friend class os::timer<QDaqLoop>;
    timer_t thread_;

    // the () operator is defined for the timer thread
    bool operator()() { return exec(); }

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

    /// Return true if this is a top level loop
    bool isTop() const { return this==topLoop(); }

    /// Return the parent loop of this loop
    QDaqLoop* parentLoop() const;

    virtual QDaqScriptEngine* loopEngine() const;

signals:
    /// This is emitted if the loop aborts due to an error.
    void abort();

public slots:

    /**
     * @brief Arm the loop.
     *
     * If this is a top level loop then this function
     * starts the timer thread after succesfully arming all
     * child jobs.
     *
     * If it is a child loop then only the arming is done.
     *
     * @return true if the loop is succesfully armed.
     */
    bool arm() { return setArmed(true); }

    /// Disarm the loop.
    void disarm() { setArmed(false); }

    /// Print loop statistics.
    QString stat();

    /**
     * @brief Create a dedicated QDaqScriptEngine.
     *
     * The created script engine will be used for running
     * script code of child jobs.
     *
     * The created script engine lives executes code
     * within the loop timer thread.
     *
     * All child jobs can obtain a pointer to this engine with
     * loopEngine().
     *
     */
    void createLoopEngine();
};

#endif


 
