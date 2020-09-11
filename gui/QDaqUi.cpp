#include "QDaqUi.h"

#include "QDaqIde.h"

#include <QTabWidget>
#include <QtUiTools/QUiLoader>
#include <QWidget>
#include <QComboBox>
#include <QListWidget>
#include <QTabWidget>
#include <QLayout>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QFileInfo>
#include <QDir>

#include "QDaqDelegates.h"
#include "QDaqWindow.h"
#include "QDaqRoot.h"


QDaqUi* QDaqUi::ui_;

QDaqUi::QDaqUi(QObject *parent) : QObject(parent), ideWindow_(0)
{
    setObjectName("ui");
    ui_ = this;
}

QDaqUi *QDaqUi::instance()
{
    return ui_;
}

QDaqIDE* QDaqUi::createIdeWindow()
{
    if (!ideWindow_) {
        ideWindow_ = new QDaqIDE;
    }
    return ideWindow_;
}

void QDaqUi::addDaqWindow(QWidget* w)
{
    if (!daqWindows_.contains(w)) {
        daqWindows_.push_back(w);
        emit daqWindowsChanged();
    }
}

void QDaqUi::removeDaqWindow(QWidget* w)
{
    if (daqWindows_.contains(w)) {
        daqWindows_.removeOne(w);
        emit daqWindowsChanged();
    }
}

/*
 *
 * G U I functions
 *
 */

void QDaqUi::bind(QDaqChannel *ch, QWidget* w)
{
    if (ch && w)
    {
        DisplayDelegate* d = new DisplayDelegate(w,ch);
        Q_UNUSED(d);
        //displayDelegates << d;
    }
}

QWidget* QDaqUi::loadUi(const QString &fname)
{
    QFile file(fname);

    if (!file.open(QFile::ReadOnly))
    {
        engine()->currentContext()->throwError(QString("Ui file %1 could not be opened.").arg(fname));
        return 0;
    }

    QUiLoader loader;
    // loader working dir = ui file dir
    QFileInfo fi(file);
    loader.setWorkingDirectory(fi.absoluteDir());
    QWidget* w = loader.load(&file);
    file.close();
    if (!w)
    {
        engine()->currentContext()->throwError(
                    QString("Error while loading file %1.\n%2").arg(fname).arg(loader.errorString())
                    );
    }

    return w;
}
QWidget*  QDaqUi::loadTopLevelUi(const QString &fname, const QString &uiName)
{
    QWidget* w = loadUi(fname);
    if (!w) return 0;

    QDaqWindow* ui = new QDaqWindow;
    ui->setWidget(w);
    ui->setObjectName(uiName);

    instance()->addDaqWindow(ui);
    return ui;
}

void QDaqUi::onUiChanged()
{
    QScriptValue uiObj = engine()->globalObject().property("ui");

    QScriptValueIterator it(uiObj);
    while (it.hasNext()) {
         it.next();
         if (it.flags() & (QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration))
             continue;
         it.setValue(QScriptValue());
     }

//    QWidgetList wl = QDaqObject::root()->daqWindows();
//    foreach (QWidget* w, wl) {
//        QScriptValue wObj = engine_->newQObject(w);
//        QString wname = w->objectName();
//        if (!wname.isEmpty()) {
//            uiObj.setProperty(wname,wObj);
//        }
//    }

//    //add ide object to list of objects available to session
//    QObject *idemw = (QObject*)QDaqObject::root()->createIdeWindow();
//    QScriptValue ideObj = engine_->newQObject(idemw);
//    QString idemv = "ideHandle";
//    uiObj.setProperty(idemv,ideObj);
}
QString QDaqUi::pluginPaths()
{
    QUiLoader l;
    return l.pluginPaths().join("\n");
}
QString QDaqUi::availableWidgets()
{
    QUiLoader l;
    return l.availableWidgets().join("\n");
}

void QDaqUi::bind(QDaqObject *obj, const QString& propertyName, QWidget* w, bool readOnly)
{
    if (!obj)
    {
        return;
    }
    const QMetaObject* metaObj = obj->metaObject();
    int idx = metaObj->indexOfProperty(propertyName.toLatin1());
    if (idx<0)
    {
        engine()->currentContext()->throwError(
            QString("%1 is not a property of %2").arg(propertyName).arg(obj->objectName())
            );
        return;
    }
    QMetaProperty p = metaObj->property(idx);
    if(!w || !w->isWidgetType())
    {
        engine()->currentContext()->throwError(
            QString("Invalid widget")
            );
        return;
    }
    WidgetVariant wv(w);
    if (!wv.canConvert(p.type()))
    {
        engine()->currentContext()->throwError(
            QString("Property %1 (%2) is not compatible with widget %3 (%4)")
            .arg(propertyName)
            .arg(p.typeName())
            .arg(w->objectName())
            .arg(w->metaObject()->className())
            );
        return;
    }

    PropertyDelegate* d = new PropertyDelegate(w,obj,p,readOnly);

    Q_UNUSED(d);


}

void QDaqUi::addItems(QComboBox* cb, const QStringList& lst)
{
    cb->addItems(lst);
}

void QDaqUi::addItems(QListWidget* cb, const QStringList& lst)
{
    cb->addItems(lst);
}


int QDaqUi::insertTab(int index, QWidget * page, const QString & label)
{
    QTabWidget* tabWidg = nullptr;
//    QWidget* CopyPage = nullptr;
//    QString pagename = page->objectName();
    QWidgetList wl = daqWindows();
      foreach (QObject* ui, wl) {
          tabWidg = ui->findChild<QTabWidget*>("tabWidget", Qt::FindDirectChildrenOnly);
          // first test version was copying an existing widget in the tab widget
          //          CopyPage = ui->findChild<QWidget*>(pagename);
      }

      tabWidg->insertTab(index, page, label);
    return index;
}
int QDaqUi::insertTab(int index,  QString uiname, const QString & label)
{
    QTabWidget* tabWidg = nullptr;
    QWidget* CopyPage = nullptr;
    CopyPage = loadUi(uiname);
    QWidgetList wl = daqWindows();
      foreach (QObject* ui, wl) {
          tabWidg = ui->findChild<QTabWidget*>("tabWidget", Qt::FindDirectChildrenOnly);
      }
      tabWidg->insertTab(index, CopyPage, label);
      return index;

}

int QDaqUi::insertTab(int index, QWidget * page, const QString & label, QTabWidget * tabWidget)
{
          tabWidget->insertTab(index, page, label);
          return index;
}

void QDaqUi::deleteTab(int currentIndex)
{
    QTabWidget* tabWidg = nullptr;
    QWidgetList wl = daqWindows();
      foreach (QObject* ui, wl) {
          tabWidg = ui->findChild<QTabWidget*>("tabWidget", Qt::FindDirectChildrenOnly);
    }
    tabWidg->removeTab(currentIndex);
}

void QDaqUi::deleteTab(int index,  QTabWidget * tabWidget)
{
          tabWidget->removeTab(index);
}

void QDaqUi::insertWidget(QWidget * parent, QWidget * child)
{
    QLayout * layout = parent->layout();

    if (!layout){
        engine()->currentContext()->throwError("Cannot add a widget to a parent with no layout");
    }
    else {
        layout->addWidget(child);
    }
}
void QDaqUi::deleteWidget(QWidget * parent, QWidget * child)
{
    // This method also deletes the child widget's layout and resizes the parent
    // If need be, these options can be broken down (there is already the
    // implementation of 'deleteItem' for layout below).
    QLayout * layout = parent->layout();
    QLayout * childlayout = child->layout();
    layout->removeWidget(child);
    layout->removeItem(childlayout);
    child->deleteLater();
    delete child->layout();
    parent->resize(parent->sizeHint());
}

//void QDaqSession::deleteItem(QWidget * parent, QLayout * childlayout)
//{
//    QLayout * layout = parent->layout();
//    layout->removeItem(childlayout);
//    childlayout->deleteLater();
//}

void QDaqUi::rename(QWidget * widget, QString newname)
{
    widget->setObjectName(newname);
}

//void QDaqSession::rename(QLayout * layout, QString newname)
//{
//    layout->setObjectName(newname);
//}

