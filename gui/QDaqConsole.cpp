#include <QCoreApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QTimer>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabBar>
#include <QToolBar>

#include "QDaqConsole.h"

#include "QDaqSession.h"

#include "QDaqRoot.h"

QDaqConsole::QDaqConsole(QDaqSession *s, QWidget *parent) : QConsoleWidget(parent), session_(s)
{
	setTabStopWidth ( 40 );
    setObjectName(QString("console%1").arg(session_->index()));
    if (session_->index()) setWindowTitle(QString("Console #%1").arg(session_->index()));
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

    connect(session_,SIGNAL(stdOut(const QString&)),this,SLOT(stdOut(const QString&)));
    connect(session_,SIGNAL(stdErr(const QString&)),this,SLOT(stdErr(const QString&)));
    connect(session_,SIGNAL(endSession()),this,SLOT(endSession()),Qt::QueuedConnection);
}

QDaqConsole::~QDaqConsole()
{
    if (session_->index()) delete session_;
}

void QDaqConsole::exec(const QString& code)
{
    session_->evaluate(code);
}
bool QDaqConsole::canEvaluate(const QString& code)
{
    return session_->canEvaluate(code);
}

void QDaqConsole::endSession()
{
    QWidget* w = window();
    w->close();
}

void QDaqConsole::closeEvent ( QCloseEvent * e )
{
    if (session_->index()==0) {
        e->accept();
        return;
    }

	bool ok = true;

    if (session_->isEvaluating())
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
            session_->abortEvaluation();
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
            session_->abortEvaluation();
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

    QScriptEngine* eng = session_->getEngine();
    QScriptValue scriptObj = eng->evaluate(lookup);

    // if the engine cannot recognize the variable return
    if (eng->hasUncaughtException()) return properties;

     // if a QObject add the named children
    if (scriptObj.isQObject())
    {
        QObject* obj = scriptObj.toQObject();

        foreach(QObject* ch, obj->children())
        {
            QString name = ch->objectName();
            if (!name.isEmpty())
                children << name;
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
                it.scriptName().toArrayIndex(&isIdx);
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

/*********************** QDaqConsoleTabWidget *********************/
QDaqConsoleTabWidget::QDaqConsoleTabWidget(QWidget *parent) : QTabWidget(parent)
{
    tabBar()->setExpanding(false);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabClose(int)));

    QToolBar* bar = new QToolBar;

    abort_ = bar->addAction(QIcon(":/images/stop.png"),"Abort script",this,SLOT(abortScript()));
    bar->addAction(QIcon(":/images/Terminal-128.png"),"Add console tab",this,SLOT(addConsole()));

    setCornerWidget(bar,Qt::TopRightCorner);

}

QSize QDaqConsoleTabWidget::sizeHint() const
{
    return QSize(600,400);
}

void QDaqConsoleTabWidget::addConsole()
{
    QDaqConsole *child;
    if (count()==0) {
        child = new QDaqConsole(QDaqObject::root()->rootSession());
    } else child = new QDaqConsole(QDaqObject::root()->newSession());
    addTab(child, child->windowTitle());
    if (count()>1) setTabsClosable(true);
}

void QDaqConsoleTabWidget::tabRemoved(int index)
{
    Q_UNUSED(index);
    if (count()==1) setTabsClosable(false);
}

void QDaqConsoleTabWidget::onTabClose(int index)
{
    if (index) {
        QWidget* w = widget(index);
        removeTab(index);
        delete w;
    }
}

QDaqConsole* QDaqConsoleTabWidget::currentConsole()
{
    return (QDaqConsole*)currentWidget();
}

void QDaqConsoleTabWidget::abortScript()
{
    QDaqConsole * c = (QDaqConsole*)currentWidget();
    if (c) c->session()->abortEvaluation();
}


