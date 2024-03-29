#ifndef QDAQUI_H
#define QDAQUI_H

#include "QDaqGlobal.h"

#include <QObject>
#include <QWidgetList>
#include <QScriptable>

class QDaqIDE;
class QDaqChannel;
class QComboBox;
class QListWidget;
class QWidget;
class QTabWidget;
class QDaqObject;
class QScriptEngine;
class QDaqSession;

class QDAQ_EXPORT QDaqUi : public QObject
{
    Q_OBJECT

public:
    explicit QDaqUi(QObject *parent = 0);

    static QDaqUi* instance();

    static void initScriptInterface(QDaqSession* s);

    void addDaqWindow(QWidget* w);
    void removeDaqWindow(QWidget* w);

    /// Return a list of QDaq top level windows.
    QWidgetList daqWindows() const { return daqWindows_; }

    /// Return a pointer to the QDaq IDE window.
    QDaqIDE* ideWindow() { return ideWindow_; }

    /// Create the QDaq IDE window and return a pointer to it.
    QDaqIDE* createIdeWindow();

protected:
    QDaqIDE* ideWindow_;

signals:
    /// Fired when a top level window is opened or closed
    void daqWindowsChanged();

private slots:
    void onNewSession(QDaqSession* s);

private:
    static QDaqUi* ui_;
    QWidgetList daqWindows_;
};

Q_DECLARE_METATYPE(QDaqUi*)

class QDAQ_EXPORT QDaqUiProto : public QObject, public QScriptable
{
    Q_OBJECT

public:
    explicit QDaqUiProto(QScriptEngine *parent);

public slots:

    // widget functions
    QWidget* loadUi(const QString& fname);
    QWidget* loadTopLevelUi(const QString& fname, const QString &uiName);
    QString pluginPaths();
    QString availableWidgets();
    void bind(QDaqChannel* ch, QWidget* w);
    void bind(QDaqObject* obj, const QString& propertyName, QWidget* w, bool readOnly = false);
    void addItems(QComboBox* cb, const QStringList& lst);
    void addItems(QListWidget* cb, const QStringList& lst);

    int insertTab(QTabWidget * tabWidget, int index, QWidget * page, const QString & label);
    void deleteTab(QTabWidget * tabWidget, int index);

    void insertWidget(QWidget * parent, QWidget * child);
    void deleteWidget(QWidget * parent, QWidget * child);

    void onUiChanged();

};

#endif // QDAQUI_H
