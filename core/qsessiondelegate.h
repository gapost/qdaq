#ifndef QSESSIONDELEGATE_H
#define QSESSIONDELEGATE_H

#include <QObject>

class QDaqSession;
class QDaqScriptAPI;
class QThread;


/*
 * This object coordinates the communication between
 * a QDaqSession and the QDaqEngine+QDaqScriptAPI
 *
 * The ROOT session runs in the main UI thread and thus
 * is the only session that may interact with the UI.
 *
 * In normal sessions the script engine runs in a separate thread + event loop.
 * Thus it can only access the QDaq objects which are thread safe.
 * In this case QSessionDelegate is the interface to the engine thread. Its slots
 * run in the separate thread while its signals are emitted either from the
 * main thread or from the separate thread.
 *
 * This design was decided due to problems with the
 * implementation of the "wait" script API function. (see comments
 * in the function definition). It is also a "cleaner"
 * solution that only the ROOT console may access the UI.
 *
 */
class QSessionDelegate : public QObject
{
    Q_OBJECT

public:
    QSessionDelegate();
    ~QSessionDelegate();

    void init(bool ownthread, QDaqSession* s);

    QThread* thread_;
    QDaqSession* session_;

public slots:

    // accepting API+self signals
    void onStdOut(const QString& str);
    void onStdErr(const QString& str);
    void onEndSession();

signals:
    // signals sent to self
    void eval(const QString&);
    void abort();
    void stdOut(const QString& str);
    void stdErr(const QString& str);
    void evalFinished();

private slots:
    // accepting self signals
    void onEval(const QString& code);
    void onAbort();
    void onEvalFinished();
};

#endif // QSESSIONDELEGATE_H
