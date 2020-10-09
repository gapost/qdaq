#ifndef _SCRIPTCONSOLE_H_
#define _SCRIPTCONSOLE_H_

#include "QDaqGlobal.h"
#include "QConsoleWidget.h"

#include <QTabWidget>

class QDaqSession;

class QDAQ_EXPORT QDaqConsole : public QConsoleWidget
{
    Q_OBJECT

    QDaqSession* session_;

public:
    QDaqConsole(QDaqSession* s, QWidget* parent = 0);
    virtual ~QDaqConsole();

    QDaqSession* session() { return session_; }

public slots:

    void endSession();
    void onRequestInput(const QString &prompt);

protected:

	virtual void closeEvent ( QCloseEvent * event );

private:
    QString multilineCode_;
};

class QDAQ_EXPORT QDaqConsoleTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    QDaqConsoleTabWidget(QWidget* parent = 0);

    QDaqConsole* currentConsole();

    virtual QSize sizeHint() const;

protected:
    virtual void tabRemoved(int index);

public slots:

    void addConsole();

private slots:
    void onTabClose(int index);
    void abortScript();

private:
    QAction* abort_;

};

#endif

