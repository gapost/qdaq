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
#include "QDaqSession.h"


QDaqUi* QDaqUi::ui_;

QDaqUi::QDaqUi(QObject *parent) : QObject(parent), ideWindow_(0)
{
    setObjectName("ui");
    ui_ = this;

    initScriptInterface( QDaqObject::root()->rootSession() );

    connect(QDaqObject::root(),SIGNAL(newSession(QDaqSession*)),
            this,SLOT(onNewSession(QDaqSession*)));
}

QDaqUi *QDaqUi::instance()
{
    return ui_;
}

QDaqIDE* QDaqUi::createIdeWindow()
{
    if (!ideWindow_) {
        ideWindow_ = new QDaqIDE;
        emit daqWindowsChanged();
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

void QDaqUi::initScriptInterface(QDaqSession *s)
{
    QScriptEngine* e = s->scriptEngine();
    QDaqUiProto* proto = new QDaqUiProto(e);
    e->setDefaultPrototype(qMetaTypeId<QDaqUi*>(),
                           e->newQObject(proto,QScriptEngine::ScriptOwnership));

    QDaqUi* ui = instance();

    QScriptValue uiObj = e->newQObject(ui,
                                       QScriptEngine::QtOwnership);

    foreach (QWidget* w, ui->daqWindows()) {
        QScriptValue wObj = e->newQObject(w);
        QString wname = w->objectName();
        if (!wname.isEmpty()) {
            uiObj.setProperty(wname,wObj);
        }
    }

    //add ide object to list of objects available to session
    if (ui->ideWindow()) {
        QScriptValue ideObj = e->newQObject(ui->ideWindow());
        uiObj.setProperty("ideHandle",ideObj);
    }

    e->globalObject().setProperty("ui",uiObj,
                                  QScriptValue::Undeletable | QScriptValue::ReadOnly);

    connect(ui,SIGNAL(daqWindowsChanged()),
            proto,SLOT(onUiChanged()));

}

void QDaqUi::onNewSession(QDaqSession *s)
{
    // initScriptInterface(s);
}

/**************** Prototype ******************/

QDaqUiProto::QDaqUiProto(QScriptEngine* p) : QObject(p)
{

}

void QDaqUiProto::bind(QDaqChannel *ch, QWidget* w)
{
    if (ch && w)
    {
        DisplayDelegate* d = new DisplayDelegate(w,ch);
        Q_UNUSED(d);
        //displayDelegates << d;
    }
}

QWidget* QDaqUiProto::loadUi(const QString &fname)
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
QWidget*  QDaqUiProto::loadTopLevelUi(const QString &fname, const QString &uiName)
{
    QWidget* w = loadUi(fname);
    if (!w) return 0;

    QDaqWindow* ui = new QDaqWindow;
    ui->setWidget(w);
    ui->setObjectName(uiName);

    QDaqUi::instance()->addDaqWindow(ui);
    return ui;
}

void QDaqUiProto::onUiChanged()
{
    QScriptEngine* e = qobject_cast<QScriptEngine*>(parent());

    if (!e) return;

    QScriptValue uiObj = e->globalObject().property("ui");
    QDaqUi* ui = QDaqUi::instance();

    QScriptValueIterator it(uiObj);
    while (it.hasNext()) {
         it.next();
         if (it.flags() & (QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration))
             continue;
         it.setValue(QScriptValue());
     }

    foreach (QWidget* w, ui->daqWindows()) {
        QScriptValue wObj = e->newQObject(w);
        QString wname = w->objectName();
        if (!wname.isEmpty()) {
            uiObj.setProperty(wname,wObj);
        }
    }

    //add ide object to list of objects available to session
    if (ui->ideWindow()) {
        QScriptValue ideObj = e->newQObject(ui->ideWindow());
        uiObj.setProperty("ideHandle",ideObj);
    }
}
QString QDaqUiProto::pluginPaths()
{
    QUiLoader l;
    return l.pluginPaths().join("\n");
}
QString QDaqUiProto::availableWidgets()
{
    QUiLoader l;
    return l.availableWidgets().join("\n");
}

void QDaqUiProto::bind(QDaqObject *obj, const QString& propertyName, QWidget* w, bool readOnly)
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

void QDaqUiProto::addItems(QComboBox* cb, const QStringList& lst)
{
    cb->addItems(lst);
}

void QDaqUiProto::addItems(QListWidget* cb, const QStringList& lst)
{
    cb->addItems(lst);
}



int QDaqUiProto::insertTab(QTabWidget * tabWidget, int index, QWidget * page, const QString & label)
{
          tabWidget->insertTab(index, page, label);
          return index;
}

void QDaqUiProto::deleteTab(QTabWidget * tabWidget, int index)
{
          tabWidget->removeTab(index);
}

void QDaqUiProto::insertWidget(QWidget * parent, QWidget * child)
{
    QLayout * layout = parent->layout();

    if (!layout){
        engine()->currentContext()->throwError("Cannot add a widget to a parent with no layout");
    }
    else {
        layout->addWidget(child);
    }
}
void QDaqUiProto::deleteWidget(QWidget * parent, QWidget * child)
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


