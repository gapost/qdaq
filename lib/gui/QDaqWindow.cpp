#include "QDaqWindow.h"
#include "QDaqIde.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QAction>
#include <QMenu>

#include "QDaqRoot.h"

QDaqWindow::QDaqWindow(QWidget *parent) : QWidget(parent), w_(0)
{
    QHBoxLayout* hl = new QHBoxLayout;
    setLayout(hl);

    actIde = new QAction(tr("Open &QDaq IDE"), this);
    //newAct->setShortcuts(QKeySequence::New);
    actIde->setStatusTip(tr("Open the QDaq development environment."));
    connect(actIde, SIGNAL(triggered()), this, SLOT(openIde()));
    actConsole = new QAction(tr("New &Console"), this);
    actConsole->setStatusTip(tr("Open a QDaq console."));
    actConsole->setEnabled(true);
    connect(actConsole, SIGNAL(triggered()), this, SLOT(openConsole()));

    sysMenu = new QMenu("QDaq System Menu",this);
    sysMenu->addAction(actIde);
    sysMenu->addAction(actConsole);

}

QDaqWindow::~QDaqWindow()
{
    QDaqObject::root()->removeDaqWindow(this);
}

void QDaqWindow::closeEvent ( QCloseEvent * e )
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, windowTitle(),
                 tr("Do you really want to close the window?"),
                    QMessageBox::Close | QMessageBox::Cancel );

    if (ret==QMessageBox::Close) e->accept();
    else e->ignore();

}

void QDaqWindow::setWidget(QWidget* w)
{
    QLayout* l = layout();
    if (w_) l->removeWidget(w_);
    l->addWidget(w);
    w_ = w;
}

void QDaqWindow::keyPressEvent (QKeyEvent * e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        int k = e->key();

        if (k==Qt::Key_F8) // Ctrl-F8
        {
            QDaqIDE* w = QDaqObject::root()->ideWindow();

            bool ide_enbl = !(w && w->isVisible());
            actIde->setEnabled(ide_enbl);

            sysMenu->popup(mapToGlobal(QPoint(0,0)));
            e->accept();
            return;
        }
    }

    QWidget::keyPressEvent(e);
}

void QDaqWindow::openIde()
{
    QDaqIDE* w = QDaqObject::root()->ideWindow();
    if (!w)  {
        w = QDaqObject::root()->createIdeWindow();
    }
    w->show();
}

void QDaqWindow::openConsole()
{

}

bool QDaqWindow::replaceWidget(QWidget *from, QWidget *to)
{
    if (!isAncestorOf(from)) return false;
    QWidget* p = from->parentWidget();
    while(p && !(p->layout())) p = p->parentWidget();
    if (!p) return false;
    QLayout* l = p->layout();
    QLayoutItem* i = l->replaceWidget(from,to);
    if (i) {
        delete i;
        delete from;
        return true;
    }
    else return false;
}
