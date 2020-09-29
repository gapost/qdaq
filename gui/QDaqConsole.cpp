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
    connect(this,SIGNAL(consoleCommand(const QString&)),this,SLOT(exec(const QString&)));
    connect(this,SIGNAL(abortEvaluation()),this,SLOT(abort()));

    this->device()->close();

    QScriptCompleter * completer = new QScriptCompleter;
    completer->setParent(this);
    completer->seScripttEngine(session_->getEngine());
    setCompleter(completer);
    // A "." triggers the completer
    QStringList tr;
    tr << ".";
    setCompletionTriggers(tr);

    writeStdOut(">> ");
    setMode(QConsoleWidget::Input);

}

QDaqConsole::~QDaqConsole()
{
    if (session_->index()) delete session_;
}

void QDaqConsole::exec(const QString& code)
{    
    multilineCode_ += code;

    if (!multilineCode_.isEmpty() && session_->canEvaluate(multilineCode_))
    {
        session_->evaluate(multilineCode_);
        multilineCode_ = "";
    }

    writeStdOut(multilineCode_.isEmpty() ? "\n>> " : "\n...> ");
    setMode(QConsoleWidget::Input);
}

void QDaqConsole::abort()
{
    session_->abortEvaluation();
    writeStdOut("\n>> ");
    setMode(QConsoleWidget::Input);
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
    qDebug() << "Request abort to " << c->windowTitle();
}


