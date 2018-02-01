#ifndef _SCRIPTCONSOLE_H_
#define _SCRIPTCONSOLE_H_

#include "core/QDaqGlobal.h"
#include "gui/QConsoleWidget.h"

class QDaqSession;

class RTLAB_GUI_EXPORT QDaqConsole : public QConsoleWidget
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

#endif

