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

        if (k==Qt::Key_C) // Ctrl-C copy
        {
            copy();
            e->accept();
            return;
        }
	}
	
	QConsoleWidget::keyPressEvent(e);
}

QStringList QDaqConsole::introspection(const QString& lookup)
{
    // list of found tokens
    QStringList properties, children, functions;

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
                    children << name;
            }
        }
    }

    // add the script properties
    {
        QScriptValue obj(scriptObj); // the object to iterate over
        while (obj.isObject()) {
            QScriptValueIterator it(obj);
            while (it.hasNext()) {
                it.next();

                // avoid array indices
                bool isIdx;
                it.name().toUInt(&isIdx);
                if (isIdx) continue;

                // avoid "hidden" properties starting with "__"
                if (it.name().startsWith("__")) continue;

                // include in list
                if (it.value().isQObject()) children << it.name();
                else if (it.value().isFunction()) functions << it.name();
                else properties << it.name();
            }
            obj = obj.prototype();
        }
    }

    children.removeDuplicates();
    children.sort(Qt::CaseInsensitive);
    functions.removeDuplicates();
    functions.sort(Qt::CaseInsensitive);
    properties.removeDuplicates();
    properties.sort(Qt::CaseInsensitive);

    children.append(properties);
    children.append(functions);

    return children;

}

/*********************** QDaqConsoleTab *********************/
QDaqConsoleTab::QDaqConsoleTab(QWidget *parent) : QTabWidget(parent)
{
    QDaqConsole *child = new QDaqConsole(QDaqObject::root()->rootSession());
    addTab(child, child->windowTitle());
    setTabsClosable(false);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabClose(int)));
}

void QDaqConsoleTab::addConsole()
{
    addConsole(new QDaqSession());
}

void QDaqConsoleTab::addConsole(QDaqSession* s)
{
    QDaqConsole *child = new QDaqConsole(s);
    addTab(child, child->windowTitle());
    setTabsClosable(true);
}

void QDaqConsoleTab::tabRemoved(int index)
{
    Q_UNUSED(index);
    if (count()==1) setTabsClosable(false);
}

void QDaqConsoleTab::onTabClose(int index)
{
    if (index) {
        QWidget* w = widget(index);
        removeTab(index);
        delete w;
    }
}
