#ifndef _SCRIPTCONSOLE_H_
#define _SCRIPTCONSOLE_H_

#include "QDaqGlobal.h"
#include "QConsoleWidget.h"

#include <QTabWidget>

class QDaqSession;

class QDAQ_EXPORT QDaqConsole : public QConsoleWidget
{
    Q_OBJECT

    QDaqSession* session;

public:
    QDaqConsole(QDaqSession* s, QWidget* parent = 0);
    virtual ~QDaqConsole();

public slots:

    void endSession();

    void flush() { flushStdOut(); }

protected:
	void exec(const QString& code);
	bool canEvaluate(const QString& code);

    QStringList introspection(const QString& lookup);

	virtual void closeEvent ( QCloseEvent * event );

    //! derived key press event
    virtual void keyPressEvent (QKeyEvent * e);
};

class QDAQ_EXPORT QDaqConsoleTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    QDaqConsoleTabWidget(QWidget* parent = 0);

protected:
    virtual void tabRemoved(int index);

public slots:

    void addConsole();

private slots:
    void onTabClose(int index);

};

class QDAQ_EXPORT QDaqConsoleTab : public QWidget
{
    Q_OBJECT

    QDaqConsoleTabWidget* tabWidget_;

public:
    QDaqConsoleTab(QWidget* parent = 0);

public slots:
    void addConsole();

};



#endif

