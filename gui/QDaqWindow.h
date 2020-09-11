#ifndef QDAQWINDOW_H
#define QDAQWINDOW_H

#include <QWidget>

#include "QDaqGlobal.h"

class QMenu;
class QAction;

class QDAQ_EXPORT QDaqWindow : public QWidget
{
    Q_OBJECT

    QWidget* w_;
    QMenu* sysMenu;
    QAction* actIde;
    QAction* actConsole;
public:
    explicit QDaqWindow(QWidget *parent = 0);
    virtual ~QDaqWindow();

    void setWidget(QWidget* w);

signals:

public slots:
    bool replaceWidget(QWidget* from, QWidget* to);

private slots:
    void openIde();
    void openConsole();

protected:
    virtual void closeEvent ( QCloseEvent * event );
    virtual void keyPressEvent (QKeyEvent * e);
};

#endif // QDAQWINDOW_H
