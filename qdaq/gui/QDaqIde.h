#ifndef _QDAQIDE_H_
#define _QDAQIDE_H_

#include <QMainWindow>

class QDaqConsole;
class QDaqScriptEditor;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QPlainTextEdit;
class QDaqObjectBrowser;
class QDaqErrorLog;

class QDaqIDE : public QMainWindow
{
    Q_OBJECT

public:
    QDaqIDE();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    QDaqConsole* newConsole();
    void rootConsole();
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
	void tabbedView();
	void windowView();
    QDaqScriptEditor *createScriptEditor();
    QDaqConsole *createQDaqConsole();
    void setActiveSubWindow(QWidget *window);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockers();
    void readSettings();
    void writeSettings();
	QPlainTextEdit* activeTextEdit();
    QDaqScriptEditor* activeScriptEditor();
    QMdiSubWindow *findEditor(const QString &fileName);

    QSignalMapper *windowMapper;
    QMdiArea *mdiArea;
    QDaqObjectBrowser* objectBrowser_;
    QDaqErrorLog* errorLog_;

    QList<QAction*> toggleDockersActions;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *newConsoleAct;
    QAction *rootConsoleAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *tabbedViewAct;
    QAction *windowViewAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif

