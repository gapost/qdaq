#include <QCoreApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QTimer>

#include "QDaqConsole.h"

#include "QDaqSession.h"

#include "QDaqRoot.h"

QDaqConsole::QDaqConsole(QDaqSession *s, QWidget *parent) : QConsoleWidget(parent), session(s)
{
	setTabStopWidth ( 40 );
    setObjectName(QString("console%1").arg(session->index()));
    if (session->index()) setWindowTitle(QString("Console #%1").arg(session->index()));
    else setWindowTitle(QString("Console #0 - Root"));

#if defined(Q_OS_MAC)
    QFont textFont = font();
    textFont.setPointSize(12);
    textFont.setFamily("Monaco");
    setFont(textFont);
#elif defined(Q_OS_UNIX)
    QFont textFont = font();
    textFont.setFamily("Monospace");
    setFont(textFont);
#elif defined(Q_OS_WIN)
    QFont textFont = font();
    textFont.setFamily("Courier New");
    setFont(textFont);
#endif

	connect(session,SIGNAL(stdOut(const QString&)),this,SLOT(stdOut(const QString&)));
	connect(session,SIGNAL(stdErr(const QString&)),this,SLOT(stdErr(const QString&)));
	connect(session,SIGNAL(endSession()),this,SLOT(endSession()),Qt::QueuedConnection);
}

QDaqConsole::~QDaqConsole()
{
    if (session->index()) delete session;
}

void QDaqConsole::exec(const QString& code)
{
	session->evaluate(code);
}
bool QDaqConsole::canEvaluate(const QString& code)
{
	return session->canEvaluate(code);
}

void QDaqConsole::endSession()
{
    QWidget* w = window();
    w->close();
}

void QDaqConsole::closeEvent ( QCloseEvent * e )
{
    if (session->index()==0) {
        e->accept();
        return;
    }

	bool ok = true;

	if (session->isEvaluating())
	{
		QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, windowTitle(),
                     tr("A script is currently executing.\n"
					    "Closing this console will terminate execution.\n"
                        "Do you want to close the console?"),
						QMessageBox::Close | QMessageBox::Cancel );
		ok = ret==QMessageBox::Close;
		if (ok) 
		{
			session->abortEvaluation();
            e->accept();
            // QCoreApplication::postEvent(parentWidget(),new QCloseEvent());
		}
        else e->ignore();
	}
	else
	{
		e->accept();	
	}

	/*if (ok)
	{
		QWidget* w = parentWidget();
		w->close();
		e->accept();
	}
	else e->ignore();*/
}

void QDaqConsole::keyPressEvent (QKeyEvent * e)
{
	if (e->modifiers() & Qt::ControlModifier)
	{
		int k = e->key();
        //if (k==Qt::Key_Cancel || k==Qt::Key_Pause) // did not work on Linux. GA 24/3/2015
        if (k==Qt::Key_Q) // Ctrl-Q aborts
        {
			session->abortEvaluation();
			e->accept();
			return;
		}
	}
	
	QConsoleWidget::keyPressEvent(e);
}

QStringList QDaqConsole::introspection(const QString& lookup)
{
    // list of found tokens
    QStringList properties;

    if (lookup.isEmpty()) return properties;

    QScriptEngine* eng = session->getEngine();
    QScriptValue scriptObj = eng->evaluate(lookup);

    // if the engine cannot recognize the variable return
    if (eng->hasUncaughtException()) return properties;

     // if a QObject add the named children
    if (scriptObj.isQObject())
    {
        QObject* obj = scriptObj.toQObject();
        QDaqObject* dobj = qobject_cast<QDaqObject*>(obj);
        if (!dobj) {
            foreach(QObject* ch, obj->children())
            {
                QString name = ch->objectName();
                if (!name.isEmpty())
                    properties << name;
            }
        }
    }

    // add the script properties
    {
        QScriptValueIterator it(scriptObj);
        while (it.hasNext()) {
             it.next();
             if (it.flags() & QScriptValue::SkipInEnumeration)
                 continue;
             properties << it.name();
         }
    }

    return properties;

}

