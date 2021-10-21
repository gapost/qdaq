#include "h5helper_v1_0.h"

#include "QDaqVector.h"
#include "QDaqObject.h"

#include <QMetaProperty>

#define CLASS_ATTR_NAME "Class"

void h5helper_v1_0::writeProperties(const QH5Group& h5obj, const QDaqObject* m_object, const QMetaObject* metaObject)
{
    // get the super-class meta-object
    const QMetaObject* super = metaObject->superClass();

    // if it is null -> we reached the top, i.e. the QObject level -> return
    if (!super) return;

    // if this is the first , write the class name
    if (metaObject==m_object->metaObject())
    {
        h5obj.write( CLASS_ATTR_NAME, QString(metaObject->className()));
    }

    // first write the properties of the super-class (this produces all properties up to the current object)
    writeProperties(h5obj, m_object, super);

    // write this class properties
    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
    {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable() && metaProperty.isStored())
        {
            QVariant value = metaProperty.read(m_object);
            if (metaProperty.isEnumType())
            {
                QMetaEnum metaEnum = metaProperty.enumerator();
                int i = *reinterpret_cast<const int *>(value.constData());
                h5obj.write(metaProperty.name(),QString(metaEnum.valueToKey(i)));
            }
            else {
                int objtype = value.userType();
                if (objtype==QVariant::Bool || objtype==QVariant::Char ||
                    objtype==QVariant::Int || objtype==QVariant::UInt)
                {
                    h5obj.write(metaProperty.name(),value.toInt());
                }
                else if (objtype==QVariant::String)
                {
                    QString S = value.toString();
                    if (S.isEmpty()) S=" ";
                    h5obj.write(metaProperty.name(),S);
                }
                else if (objtype==QVariant::StringList)
                {
                    QStringList S = value.toStringList();
                    if (!S.isEmpty())
                    h5obj.write(metaProperty.name(),S);
                }
                else if (objtype==QVariant::Double) h5obj.write(metaProperty.name(),value.toDouble());
                else if (objtype==qMetaTypeId<QDaqVector>())
                    h5obj.write(metaProperty.name(),value.value<QDaqVector>());
                else if (objtype==qMetaTypeId<QDaqObject*>())
                    write(h5obj,metaProperty.name(),value.value<QDaqObject*>());
                else if (objtype==qMetaTypeId<QDaqObjectList>())
                    write(h5obj,metaProperty.name(),value.value<QDaqObjectList>());
            }
        }
    }

    // write the object's dynamic properties
    if (metaObject==m_object->metaObject()) writeDynamicProperties(h5obj,m_object);

}

void h5helper_v1_0::writeDynamicProperties(const QH5Group& h5obj, const QDaqObject* m_object)
{
    if (!m_object->dynamicPropertyNames().isEmpty()) {
        foreach(const QByteArray& ba, m_object->dynamicPropertyNames())
        {
            QVariant v = m_object->property(ba.constData());
            h5obj.write(ba.constData(),v.toString());
        }
    }
}

void h5helper_v1_0::readProperties(const QH5Group& h5obj, QDaqObject* obj)
{
    // get the meta-object
    const QMetaObject* metaObject = obj->metaObject();

    // the "class" and "objectName" properties have been read when the obj was constructed
    // start at index = 2
    for (int idx = 2; idx < metaObject->propertyCount(); idx++)
    {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isWritable() && metaProperty.isStored())
        {
            if (metaProperty.isEnumType())
            {
                QMetaEnum metaEnum = metaProperty.enumerator();
                QString key;
                if (h5obj.read(metaProperty.name(),key))
                {
                    int v = metaEnum.keyToValue(key.toLatin1().constData());
                    if (v>=0)
                        metaProperty.write(obj,QVariant(v));
                }
            }
            else {
                int objtype = metaProperty.userType();
                if (objtype==QVariant::Bool || objtype==QVariant::Char ||
                    objtype==QVariant::Int || objtype==QVariant::UInt)
                {
                    int v;
                    if (h5obj.read(metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(objtype,&v));
                }
                else if (objtype==QVariant::String)
                {
                    QString v;
                    if (h5obj.read(metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==QVariant::StringList)
                {
                    QStringList v;
                    if (h5obj.read(metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==QVariant::Double)
                {
                    double v;
                    if (h5obj.read(metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==qMetaTypeId<QDaqVector>())
                {
                    QDaqVector v;
                    if (h5obj.read(metaProperty.name(),v))
                        metaProperty.write(obj,QVariant::fromValue(v));
                }
                else if (objtype==qMetaTypeId<QDaqObject*>())
                {
                    QString path;
                    if (h5obj.read(metaProperty.name(),path))
                        deferObjPtrRead(obj,metaProperty.name(),path);
                }
                else if (objtype==qMetaTypeId<QDaqObjectList>())
                {
                    QStringList pathList;
                    if (h5obj.read(metaProperty.name(),pathList))
                        deferObjPtrRead(obj,metaProperty.name(),pathList);
                }
            }
        }
    }

    readDynamicProperties(h5obj,obj);
}

void h5helper_v1_0::readDynamicProperties(const QH5Group& h5obj, QDaqObject* m_object)
{
    Q_UNUSED(h5obj);
    Q_UNUSED(m_object);
}





