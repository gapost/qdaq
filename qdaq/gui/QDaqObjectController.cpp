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
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"

#include "QDaqTypes.h"

int isRtType(const QVariant& v)
{
    int ret = 0;
    if (v.canConvert<QDaqIntVector>()) ret = 1;
    else if (v.canConvert<QDaqUintVector>()) ret = 2;
    else if (v.canConvert<QDaqDoubleVector>()) ret = 3;
    return ret;
}

template <class T>
QString qdaqTypeToString(const QVariant& var)
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
QString qdaqTypeToString(const QVariant& var, int type)
{
    switch (type)
    {
    case 1: return qdaqTypeToString<QDaqIntVector>(var);
    case 2: return qdaqTypeToString<QDaqUintVector>(var);
    case 3: return qdaqTypeToString<QDaqDoubleVector>(var);
    default: return QString();
    }
}

class QDaqObjectControllerPrivate
{
    QDaqObjectController *q_ptr;
    Q_DECLARE_PUBLIC(QDaqObjectController)
public:

    void addClassProperties(const QMetaObject *metaObject);
    void updateClassProperties(const QMetaObject *metaObject, bool recursive);
    void saveExpandedState();
    void restoreExpandedState();
    void slotValueChanged(QtProperty *property, const QVariant &value);
    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int value) const;

    QObject                  *m_object;

    QMap<const QMetaObject *, QtProperty *> m_classToProperty;
    QMap<QtProperty *, const QMetaObject *> m_propertyToClass;
    QMap<QtProperty *, int>     m_propertyToIndex;
    QMap<const QMetaObject *, QMap<int, QtVariantProperty *> > m_classToIndexToProperty;

    QMap<QtProperty *, bool>    m_propertyToExpanded;

    QList<QtProperty *>         m_topLevelProperties;

    QtAbstractPropertyBrowser    *m_browser;
    QtVariantPropertyManager *m_manager;
    QtVariantPropertyManager *m_readOnlyManager;
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
                    subProperty->setValue(metaProperty.read(m_object));
                }
            }
        }
    }
}

/*struct Var2Int
{
	int v;
};

Q_DECLARE_METATYPE(Var2Int)
*/
void QDaqObjectControllerPrivate::addClassProperties(const QMetaObject *metaObject)
{
    //if (!metaObject)
	if (metaObject->superClass()==0) // stop at QObject
        return;

    addClassProperties(metaObject->superClass());

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        QString className = QLatin1String(metaObject->className());
        classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
        m_classToProperty[metaObject] = classProperty;
        m_propertyToClass[classProperty] = metaObject;

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;
            QVariant var;
            //int rtType;
            if (!metaProperty.isReadable()) {
                subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else if (metaProperty.isEnumType()) {
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
                    subProperty->setValue(flagToInt(metaEnum, metaProperty.read(m_object).toInt()));
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
					int i = *reinterpret_cast<const int *>(metaProperty.read(m_object).constData());
                    subProperty->setValue(enumToInt(metaEnum, i));
               }
            } else if (m_manager->isPropertyTypeSupported(type)) {
                if (!metaProperty.isWritable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                else
                    subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                subProperty->setValue(metaProperty.read(m_object));
			} else if (type == QVariant::UInt) {
				type = QVariant::Int;
                if (!metaProperty.isWritable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                else
                    subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                subProperty->setValue(metaProperty.read(m_object));
				subProperty->setAttribute("minimum",QVariant((int)0));
           /* } else if (rtType = isRtType(var = metaProperty.read(m_object))) {
                if (!metaProperty.isWritable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                else
                    subProperty = m_manager->addProperty(type, QLatin1String(metaProperty.name()));
                subProperty->setValue(qdaqTypeToString(var,rtType));*/
            } else { // could not convert the type
                subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            classProperty->addSubProperty(subProperty);
            m_propertyToIndex[subProperty] = idx;
            m_classToIndexToProperty[metaObject][idx] = subProperty;
        }
    } else {
        updateClassProperties(metaObject, false);
    }

    m_topLevelProperties.append(classProperty);
    m_browser->addProperty(classProperty);
}

void QDaqObjectControllerPrivate::saveExpandedState()
{

}

void QDaqObjectControllerPrivate::restoreExpandedState()
{

}

void QDaqObjectControllerPrivate::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (!m_propertyToIndex.contains(property))
        return;

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

    d_ptr->m_readOnlyManager = new QtVariantPropertyManager(this);
    d_ptr->m_manager = new QtVariantPropertyManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d_ptr->m_browser->setFactoryForManager(d_ptr->m_manager, factory);

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

void QDaqObjectController::setObject(QObject *object)
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

    d_ptr->restoreExpandedState();
}

QObject *QDaqObjectController::object() const
{
    return d_ptr->m_object;
}

void QDaqObjectController::updateProperties()
{
	d_ptr->updateClassProperties(object()->metaObject(), true);
}



