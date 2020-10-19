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
#include <QDebug>

#include "QDaqConsole.h"

#include "QDaqSession.h"

#include "QDaqRoot.h"

#include "qconsolewidget/examples/scriptconsole/qscriptcompleter.h"

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

    connect(session_,SIGNAL(stdOut(const QString&)),this,SLOT(writeStdOut(const QString&)));
    connect(session_,SIGNAL(stdErr(const QString&)),this,SLOT(writeStdErr(const QString&)));
    connect(session_,SIGNAL(endSession()),this,SLOT(endSession()),Qt::QueuedConnection);
    connect(session_,SIGNAL(requestInput(const QString&)),this,SLOT(onRequestInput(const QString&)),Qt::QueuedConnection);

    connect(this,SIGNAL(consoleCommand(QString)),session_,SLOT(eval(QString)));
    connect(this,SIGNAL(abortEvaluation()),session_,SLOT(abort()));

    // TODO : remember to change in qconsolewidget:
    // device should not be open
    this->device()->close();

    QScriptCompleter * completer = new QScriptCompleter;
    completer->setParent(this);
    completer->seScripttEngine(session_->scriptEngine());
    setCompleter(completer);
    // A "." triggers the completer
    QStringList tr;
    tr << ".";
    setCompletionTriggers(tr);
}

QDaqConsole::~QDaqConsole()
{
    if (session_->index()) delete session_;
}

void QDaqConsole::onRequestInput(const QString& prompt)
{
    writeStdOut(prompt);
    setMode(QConsoleWidget::Input);
}

void QDaqConsole::endSession()
{
    // TODO : fix the endSession behavior
    // QWidget* w = window();
    // w->close();
}

void QDaqConsole::closeEvent ( QCloseEvent * e )
{
//    if (session_->index()==0) {
//        e->accept();
//        return;
//    }

	bool ok = true;
    if (session_->daqEngine()->isEvaluating())
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
            //session_->abortEvaluation();
            emit abortEvaluation();
            e->accept();
            // QCoreApplication::postEvent(parentWidget(),new QCloseEvent());
		}
        else e->ignore();
	}
	else
	{
		e->accept();	
	}
}


/*********************** QDaqConsoleTabWidget *********************/
QDaqConsoleTabWidget::QDaqConsoleTabWidget(QWidget *parent) : QTabWidget(parent)
{
    tabBar()->setExpanding(false);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabClose(int)));

    QToolBar* bar = new QToolBar;

    abort_ = bar->addAction("Abort script",this,SLOT(abortScript()));
    QIcon ico = QIcon::fromTheme("process-stop");
    if (!ico.isNull())
            abort_->setIcon(ico);

    QAction* a = bar->addAction("Add console tab",this,SLOT(addConsole()));
    ico = QIcon::fromTheme("utilities-terminal");
    if (!ico.isNull())
            a->setIcon(ico);

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

    connect(child->session(),SIGNAL(endSession()),this,SLOT(onEndSession()));

    addTab(child, child->windowTitle());
    if (count()>1) setTabsClosable(true);
    setCurrentWidget(child);
    child->setFocus();
    if (count()>1) child->onRequestInput(">> ");
}

void QDaqConsoleTabWidget::tabRemoved(int index)
{
    Q_UNUSED(index);
    if (count()==1) setTabsClosable(false);
}

void QDaqConsoleTabWidget::onTabClose(int index)
{
    if (index) {
        QDaqConsole* c = (QDaqConsole*)widget(index);

        bool ok = true;
        if (c->session()->daqEngine()->isEvaluating())
        {
            QMessageBox::StandardButton ret;
            ret = QMessageBox::warning(this, windowTitle(),
                         tr("A script is currently executing.\n"
                            "Closing this console will terminate execution.\n"
                            "Do you want to close the console?"),
                            QMessageBox::Close | QMessageBox::Cancel );
            ok = ret==QMessageBox::Close;
        }

        if (ok)
        {
            removeTab(index);
            delete c;
            currentConsole()->setFocus();
        }
    }  
}

void QDaqConsoleTabWidget::onEndSession()
{
    // get the session that wants to end
    QDaqSession* s = qobject_cast<QDaqSession*>(sender());
    if (!s) return;
    if (s->index()==0)
    {
        return;
    }
    // find the tab
    for(int i = 0; i<count(); i++)
    {
        QDaqConsole* c = qobject_cast<QDaqConsole*>(widget(i));
        if (c && c->session()==s)
        {
            removeTab(i);
            currentConsole()->setFocus();
            return;
        }

    }
}

QDaqConsole* QDaqConsoleTabWidget::currentConsole()
{
    return (QDaqConsole*)currentWidget();
}

void QDaqConsoleTabWidget::abortScript()
{
    QDaqConsole * c = (QDaqConsole*)currentWidget();
    if (c) emit c->abortEvaluation();

}


