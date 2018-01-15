#ifndef _RTSESSION_H_
#define _RTSESSION_H_

#include "QDaqGlobal.h"

#include <QObject>
#include <QScriptValue>
#include <QScriptProgram>
#include <QStringList>

class QScriptEngine;
class QScriptEngineDebugger;
class QScriptContext;
class QTimer;

class RTLAB_BASE_EXPORT QDaqScriptEngine : public QObject
{
protected:
	QScriptEngine* engine_;

public:
    explicit QDaqScriptEngine(QObject* parent);

    QScriptEngine* getEngine() const { return engine_; }

	bool canEvaluate(const QString&);
	bool isEvaluating() const;
	bool evaluate(const QScriptProgram& program, QString& ret);
	bool evaluate(const QString& program, QString &ret);
	void abortEvaluation();

private:
	static QScriptValue scriptConstructor(QScriptContext *context, QScriptEngine *engine, const QMetaObject *metaObj);

};

#include <set>

class RTLAB_BASE_EXPORT QDaqSession : public QDaqScriptEngine
{
    Q_OBJECT

protected:
    QTimer* wait_timer_;
    bool wait_aborted_;

    int id_;

    typedef std::set<int> id_cont_t_;
    id_cont_t_ ids;

public:
    QDaqSession(QObject* parent = 0);
    virtual ~QDaqSession(void);
    void evaluate(const QString& program);
    void abortEvaluation();

public slots:
    void quit();
    void exec(const QString& fname);
    void print(const QString& str);
    void wait(uint ms);
    void textSave(const QString& str, const QString& fname);
    QString textLoad(const QString& fname);

    // file and folder
    QString pwd();
    bool cd(const QString& path);
    QStringList dir(const QStringList& filters = QStringList());
    QStringList dir(const QString& filter);
    bool isDir(const QString& name);

signals:
    void stdOut(const QString&);
    void stdErr(const QString&);
    void endSession();

};



#endif


