#ifndef QDAQUI_H
#define QDAQUI_H

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

class QDaqUi : public QObject, public QScriptable
{
    Q_OBJECT
public:
    explicit QDaqUi(QObject *parent = 0);

    static QDaqUi* instance();

    void addDaqWindow(QWidget* w);
    void removeDaqWindow(QWidget* w);

    /// Return a list of QDaq top level windows.
    QWidgetList daqWindows() const { return daqWindows_; }

    /// Return a pointer to the QDaq IDE window.
    QDaqIDE* ideWindow() { return ideWindow_; }

    /// Create the QDaq IDE window and return a pointer to it.
    QDaqIDE* createIdeWindow();

    // widget functions
    QWidget* loadUi(const QString& fname);
    QWidget* loadTopLevelUi(const QString& fname, const QString &uiName);
    QString pluginPaths();
    QString availableWidgets();
    void bind(QDaqChannel* ch, QWidget* w);
    void bind(QDaqObject* obj, const QString& propertyName, QWidget* w, bool readOnly = false);
    void addItems(QComboBox* cb, const QStringList& lst);
    void addItems(QListWidget* cb, const QStringList& lst);

    void deleteTab(int currentIndex);
    int insertTab(int index, QWidget * page, const QString & label);
    int insertTab(int index, QString uiname, const QString & label);
    int insertTab(int index, QWidget * page, const QString & label, QTabWidget * tabWidget);
    void insertWidget(QWidget * parent, QWidget * child);
    void deleteWidget(QWidget * parent, QWidget * child);
    void deleteTab(int index,  QTabWidget * tabWidget);
    void rename(QWidget * widget, QString newname);

protected:
    QDaqIDE* ideWindow_;

signals:
    /// Fired when a top level window is opened or closed
    void daqWindowsChanged();

public slots:
    void onUiChanged();

private:
    static QDaqUi* ui_;
    QWidgetList daqWindows_;
};

#endif // QDAQUI_H
