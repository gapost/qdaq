/****************************************************************************
**
** This file is part of a Qt Solutions component.
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** 
** Contact:  Qt Software Information (qt-info@nokia.com)
** 
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** 
****************************************************************************/

#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>

#include "QDaqObjectController.h"
#include "variantmanager.h"

#include "qtgroupboxpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"

#include "QDaqTypes.h"
#include "QDaqObject.h"

bool isQDaqType(int type) {
    return type == qMetaTypeId<QDaqObject*>() ||
            type == qMetaTypeId<QDaqObjectList>() ||
            type == qMetaTypeId<QDaqIntVector>() ||
            type == qMetaTypeId<QDaqUintVector>() ||
            type == qMetaTypeId<QDaqVector>() ||
            type == qMetaTypeId<QStringList>() ||
            type == qMetaTypeId<QVariantList>();

    return false;
}

bool isQDaqType(const QVariant& v) {
    int type = v.userType();

    if (type == qMetaTypeId<QDaqObject*>() ||
            type == qMetaTypeId<QDaqObjectList>() ||
            type == qMetaTypeId<QDaqIntVector>() ||
            type == qMetaTypeId<QDaqUintVector>() ||
            type == qMetaTypeId<QDaqVector>() ||
            type == qMetaTypeId<QStringList>() ||
            type == qMetaTypeId<QVariantList>()) return true;

    if (type==QMetaType::QObjectStar) {
        QObject* obj = v.value<QObject*>();
        QDaqObject* dacobj = qobject_cast<QDaqObject*>(obj);
        if (dacobj) return true;
    }

    return false;
}

template <class T>
QString qdaqVectorTypeToString(const QVariant& var)
{
    T v = var.value<T>();
    QString str('[');
    for(int i=0; i<v.size(); i++)
    {
        str += QString::number(v[i]);
        if (i<v.size()-1) str += ',';
    }
    str += ']';
    return str;
}
QString qdaqTypeToString(const QVariant& v)
{
    int type = v.userType();
    if (type == qMetaTypeId<QDaqIntVector>()) return qdaqVectorTypeToString<QDaqIntVector>(v);
    else if (type == qMetaTypeId<QDaqUintVector>()) return qdaqVectorTypeToString<QDaqUintVector>(v);
    else if (type == qMetaTypeId<QDaqVector>()) return qdaqVectorTypeToString<QDaqVector>(v);
    else if (type == QMetaType::QObjectStar) {
        QDaqObject* obj = qobject_cast<QDaqObject*>(v.value<QObject*>());
        if (obj) return obj->path();
        else return QString();
    }
    else if (type == qMetaTypeId<QDaqObject*>()) {
        QDaqObject* obj = v.value<QDaqObject*>();
        if (obj) return obj->path();
        else return QString();
    }
    else if (type == qMetaTypeId<QDaqObjectList>()) {
        QDaqObjectList lst = v.value<QDaqObjectList>();
        QString str('[');
        for(int i=0; i<lst.size(); i++) {
            QDaqObject* obj = lst.at(i);
            if (obj) str += obj->path();
            else str += "0x0";
            if (i<lst.size()-1) str += ",";
        }
        str += "]";
        return str;
    }
    else if (type == QMetaType::QStringList) {
        QStringList lst = v.value<QStringList>();
        QString str('[');
        for(int i=0; i<lst.size(); i++) {
            QString s = lst.at(i);
            str += s;
            if (i<lst.size()-1) str += ",";
        }
        str += "]";
        return str;
    }
    else if (type == QMetaType::QVariantList) {
        QVariantList lst = v.value<QVariantList>();
        QString str('[');
        for(int i=0; i<lst.size(); i++) {
            QString s = lst.at(i).toString();
            str += s;
            if (i<lst.size()-1) str += ",";
        }
        str += "]";
        return str;
    }
    else return QString();
}



VariantManager::VariantManager(QObject *parent)
    : QtVariantPropertyManager(parent)
{
}

VariantManager::~VariantManager()
{

}

QVariant VariantManager::value(const QtProperty *property) const
{
return QtVariantPropertyManager::value(property);
}

int VariantManager::valueType(int propertyType) const
{
    if (isQDaqType(propertyType))
        return propertyType;
    return QtVariantPropertyManager::valueType(propertyType);
}

bool VariantManager::isPropertyTypeSupported(int propertyType) const
{
    if (isQDaqType(propertyType))
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString VariantManager::valueText(const QtProperty *property) const
{
    QVariant v = value(property);
    if (isQDaqType(v)) {
        QString str = qdaqTypeToString(v);
        return str;
    }
    return QtVariantPropertyManager::valueText(property);
}

void VariantManager::setValue(QtProperty *property, const QVariant &val)
{
   QtVariantPropertyManager::setValue(property, val);
   //QVariant chk = value(property);
   //Q_ASSERT(val==chk);
}

void VariantManager::initializeProperty(QtProperty *property)
{
QtVariantPropertyManager::initializeProperty(property);
}

void VariantManager::uninitializeProperty(QtProperty *property)
{
QtVariantPropertyManager::uninitializeProperty(property);
}

class QDaqObjectControllerPrivate
{
    QDaqObjectController *q_ptr;
    Q_DECLARE_PUBLIC(QDaqObjectController)
public:

    void addClassProperties(const QMetaObject *metaObject);
    void updateClassProperties(const QMetaObject *metaObject, bool recursive);

    void addDynProperties();
    void updateDynProperties();

    void saveExpandedState();
    void restoreExpandedState();
    void slotValueChanged(QtProperty *property, const QVariant &value);
    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int value) const;

    QDaqObject                  *m_object;

    QMap<const QMetaObject *, QtProperty *> m_classToProperty;
    QMap<QtProperty *, const QMetaObject *> m_propertyToClass;
    QMap<QtProperty *, int>     m_propertyToIndex;
    QMap<const QMetaObject *, QMap<int, QtVariantProperty *> > m_classToIndexToProperty;

    QMap<QtProperty *, bool>    m_propertyToExpanded;

    QMap<QByteArray, QtVariantProperty *> m_dynPropNameToProperty;
    QMap<QtVariantProperty *, QByteArray> m_propertyToDynPropName;

    QList<QtProperty *>         m_topLevelProperties;

    QtAbstractPropertyBrowser    *m_browser;
    VariantManager *m_manager;
    VariantManager *m_readOnlyManager;
};

int QDaqObjectControllerPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            if (value == enumValue)
                return pos;
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int QDaqObjectControllerPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    if (intValue >= values.count())
        return -1;
    return values.at(intValue);
}

bool QDaqObjectControllerPrivate::isSubValue(int value, int subValue) const
{
    if (value == subValue)
        return true;
    int i = 0;
    while (subValue) {
        if (!(value & (1 << i))) {
            if (subValue & 1)
                return false;
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool QDaqObjectControllerPrivate::isPowerOf2(int value) const
{
    while (value) {
        if (value & 1) {
            return value == 1;
        }
        value = value >> 1;
    }
    return false;
}

int QDaqObjectControllerPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    if (!flagValue)
        return 0;
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value))
                intValue |= (1 << pos);
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int QDaqObjectControllerPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp) {
        if (i >= values.count())
            return -1;
        if (temp & 1)
            flagValue |= values.at(i);
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

void QDaqObjectControllerPrivate::updateClassProperties(const QMetaObject *metaObject, bool recursive)
{
    if (!metaObject)
        return;

    if (recursive)
        updateClassProperties(metaObject->superClass(), recursive);

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty)
        return;

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable()) {
            if (m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx)) {
                QtVariantProperty *subProperty = m_classToIndexToProperty[metaObject][idx];
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType())
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
                    else
					{
						int i = *reinterpret_cast<const int *>(metaProperty.read(m_object).constData());
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), i));
					}
                } else {
                    QVariant v = metaProperty.read(m_object);
                    if (isQDaqType(v)) subProperty->setValue(qdaqTypeToString(v));
                    else subProperty->setValue(v);
                }
            }
        }
    }
}

void QDaqObjectControllerPrivate::updateDynProperties()
{
    QByteArrayList dynProps = m_object->dynamicPropertyNames();
    foreach(const QByteArray& propName, dynProps) {
        if (m_dynPropNameToProperty.contains(propName)) {
            QtVariantProperty *subProperty = m_dynPropNameToProperty[propName];
            QVariant var = m_object->property(propName.constData());
            if (isQDaqType(var)) subProperty->setValue(qdaqTypeToString(var));
            else subProperty->setValue(var);
        }
    }
}

void QDaqObjectControllerPrivate::addClassProperties(const QMetaObject *metaObject)
{
    //if (!metaObject)
	if (metaObject->superClass()==0) // stop at QObject
        return;

    addClassProperties(metaObject->superClass());

    QString className = QLatin1String(metaObject->className());
    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
        m_classToProperty[metaObject] = classProperty;
        m_propertyToClass[classProperty] = metaObject;

        int idx = metaObject->propertyOffset();
        for (; idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;

            if (!metaProperty.isReadable()) {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else {
                QVariant var = metaProperty.read(m_object);
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType()) {
                        subProperty = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), QLatin1String(metaProperty.name()));
                        QMetaEnum metaEnum = metaProperty.enumerator();
                        QMap<int, bool> valueMap;
                        QStringList flagNames;
                        for (int i = 0; i < metaEnum.keyCount(); i++) {
                            int value = metaEnum.value(i);
                            if (!valueMap.contains(value) && isPowerOf2(value)) {
                                valueMap[value] = true;
                                flagNames.append(QLatin1String(metaEnum.key(i)));
                            }
                            subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                            subProperty->setValue(flagToInt(metaEnum, var.toInt()));
                        }
                    } else {
                        subProperty = m_manager->addProperty(QtVariantPropertyManager::enumTypeId(), QLatin1String(metaProperty.name()));
                        QMetaEnum metaEnum = metaProperty.enumerator();
                        QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
                        QStringList enumNames;
                        for (int i = 0; i < metaEnum.keyCount(); i++) {
                            int value = metaEnum.value(i);
                            if (!valueMap.contains(value)) {
                                valueMap[value] = true;
                                enumNames.append(QLatin1String(metaEnum.key(i)));
                            }
                        }
                        subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                        int i = *reinterpret_cast<const int *>(var.constData());
                        subProperty->setValue(enumToInt(metaEnum, i));
                    }
                } else if (isQDaqType(var)) {
                    // TODO
                    //                if (!metaProperty.isWritable())
                    //                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                    //                if (!metaProperty.isDesignable())
                    //                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                    //                else
                    //                    subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                    //                subProperty->setValue(var);
                    subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                    subProperty->setValue(qdaqTypeToString(var));
                    subProperty->setEnabled(false);
                } else if (m_manager->isPropertyTypeSupported(type)) {
                    if (!metaProperty.isWritable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                    if (!metaProperty.isDesignable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                    else
                        subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                    subProperty->setValue(var);
                } else if (type == QVariant::UInt) {
                    type = QVariant::Int;
                    if (!metaProperty.isWritable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                    if (!metaProperty.isDesignable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                    else
                        subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                    subProperty->setValue(var);
                    subProperty->setAttribute("minimum",QVariant((int)0));
                } else { // could not convert the type
                    subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                    subProperty->setValue(QLatin1String(var.typeName()));
                    subProperty->setEnabled(false);
                }
                classProperty->addSubProperty(subProperty);
                m_propertyToIndex[subProperty] = idx;
                m_classToIndexToProperty[metaObject][idx] = subProperty;
            }
        }

    } else {
        updateClassProperties(metaObject, false);
    }

    m_topLevelProperties.append(classProperty);
    m_browser->addProperty(classProperty);
}

void QDaqObjectControllerPrivate::addDynProperties()
{
    QtProperty *classProperty = m_classToProperty.value(&QDaqObject::staticMetaObject);

    QList<QtVariantProperty*> lst = m_dynPropNameToProperty.values();
    QListIterator<QtVariantProperty *> it(lst);
    while (it.hasNext()) {
        classProperty->removeSubProperty(it.next());
    }
    m_dynPropNameToProperty.clear();
    m_propertyToDynPropName.clear();

    QByteArrayList dynProps = m_object->dynamicPropertyNames();
    QStringList sorted;
    foreach(const QByteArray& propName, dynProps) sorted << QString(propName);
    sorted.sort();

    foreach(const QString& propNameStr, sorted) {
        QByteArray propName = propNameStr.toLatin1();
        QVariant var = m_object->property(propName.constData());
        int type = var.userType();
        QtVariantProperty *subProperty = 0;
        if (isQDaqType(var)) {
            subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(propName));
            subProperty->setValue(qdaqTypeToString(var));
            subProperty->setEnabled(false);
        } else if (m_manager->isPropertyTypeSupported(type)) {
            subProperty = m_manager->addProperty(type, QLatin1String(propName));
            subProperty->setValue(var);
        } else if (type == QVariant::UInt) {
            type = QVariant::Int;
            subProperty = m_manager->addProperty(type, QLatin1String(propName));
            subProperty->setValue(var);
            subProperty->setAttribute("minimum",QVariant((int)0));
        } else { // could not convert the type
            subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(propName));
            QString str = var.toString();
            if (str.isEmpty()) {
                subProperty->setValue(QLatin1String(var.typeName()));
                subProperty->setEnabled(false);
            } else subProperty->setValue(str);

        }
        classProperty->addSubProperty(subProperty);
        m_dynPropNameToProperty[propName] = subProperty;
        m_propertyToDynPropName[subProperty] = propName;
    }
}

void QDaqObjectControllerPrivate::saveExpandedState()
{

}

void QDaqObjectControllerPrivate::restoreExpandedState()
{

}

void QDaqObjectControllerPrivate::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (m_propertyToIndex.contains(property)) {


    int idx = m_propertyToIndex.value(property);

    const QMetaObject *metaObject = m_object->metaObject();
    QMetaProperty metaProperty = metaObject->property(idx);
    if (metaProperty.isEnumType()) {
        if (metaProperty.isFlagType())
            metaProperty.write(m_object, intToFlag(metaProperty.enumerator(), value.toInt()));
        else
            metaProperty.write(m_object, intToEnum(metaProperty.enumerator(), value.toInt()));
    } else {
        metaProperty.write(m_object, value);
    }

    updateClassProperties(metaObject, true);

    return;
    }

    if (m_propertyToDynPropName.contains((QtVariantProperty*)property))
    {
        QByteArray propName = m_propertyToDynPropName.value((QtVariantProperty*)property);
        m_object->setProperty(propName,value);
        updateDynProperties();
    }
}

///////////////////

QDaqObjectController::QDaqObjectController(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = new QDaqObjectControllerPrivate;
    d_ptr->q_ptr = this;

    d_ptr->m_object = 0;
/*
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    d_ptr->m_browser = new QtGroupBoxPropertyBrowser(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(scroll);
    scroll->setWidget(d_ptr->m_browser);
*/
    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->setRootIsDecorated(false);
    d_ptr->m_browser = browser;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d_ptr->m_browser);

    d_ptr->m_readOnlyManager = new VariantManager(this);
    d_ptr->m_manager = new VariantManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d_ptr->m_browser->setFactoryForManager((QtVariantPropertyManager*) d_ptr->m_manager, factory);

    connect(d_ptr->m_manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(valueChanged(QtProperty *, const QVariant &)));
//                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
}

void QDaqObjectController::valueChanged(QtProperty* p, const QVariant& v)
{
	d_ptr->slotValueChanged(p,v);
}


QDaqObjectController::~QDaqObjectController()
{
    delete d_ptr;
}

void QDaqObjectController::setObject(QDaqObject *object)
{
    if (d_ptr->m_object == object)
        return;

    if (d_ptr->m_object) {
        d_ptr->saveExpandedState();
        QListIterator<QtProperty *> it(d_ptr->m_topLevelProperties);
        while (it.hasNext()) {
            d_ptr->m_browser->removeProperty(it.next());
        }
        d_ptr->m_topLevelProperties.clear();
    }

    d_ptr->m_object = object;

    if (!d_ptr->m_object)
        return;

    d_ptr->addClassProperties(d_ptr->m_object->metaObject());
    d_ptr->addDynProperties();

    d_ptr->restoreExpandedState();
}

QDaqObject *QDaqObjectController::object() const
{
    return d_ptr->m_object;
}

void QDaqObjectController::updateProperties()
{
	d_ptr->updateClassProperties(object()->metaObject(), true);
}

void QDaqObjectController::updateDynamicProperties()
{
    d_ptr->addDynProperties();
}




