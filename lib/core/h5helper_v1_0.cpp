#include "h5helper_v1_0.h"

#include "QDaqTypes.h"
#include "QDaqObject.h"

#include <QMetaProperty>

#define CLASS_ATTR_NAME "Class"

void h5helper_v1_0::write(CommonFG* h5obj, const char* name, const QString& S)
{
    int len = S.length();
    if (len==0) return;
    StrType type(PredType::C_S1, len);
    hsize_t dims = 1;
    DataSpace space(1,&dims);
    DataSet ds = h5obj->createDataSet(name, type, space);
    ds.write(S.toLatin1().constData(),type);
}
void h5helper_v1_0::write(CommonFG* h5obj, const char* name, const QStringList& S)
{
    int ncols = 0, nrows = S.size();
    foreach(const QString& s, S) if (s.length() > ncols) ncols = s.length();

    QVector<char> wbuff(ncols*nrows);
    for(int i=0; i<nrows; i++)
    {
        QByteArray ba = S.at(i).toLatin1();
        memcpy(wbuff.data() + (i*ncols),ba.constData(),ba.length());
    }

    StrType type(PredType::C_S1, ncols);

    /* Create dataspace for datasets */
    hsize_t dims = nrows;
    DataSpace space(1, &dims);

    /* Create a dataset */
    DataSet dataset = h5obj->createDataSet(name, type, space);

    /* Write dataset to disk */
    dataset.write(wbuff.constData(), type, space);

    /* Close Dataset */
    dataset.close();
}


bool h5helper_v1_0::read(CommonFG* h5obj, const char* name, QString& str)
{
    if (!h5exist_ds(h5obj,name)) {
        pushWarning(QString("DataSet '%1' not found in group '%2'")
                .arg(name).arg(groupName(h5obj)));
        return false;
    }
    DataSet ds = h5obj->openDataSet( name );
    H5T_class_t type_class = ds.getTypeClass();
    if (type_class==H5T_STRING)
    {
        StrType st = ds.getStrType();
        int sz = st.getSize();
        QByteArray buff(sz,'0');
        ds.read(buff.data(),st);
        str = QString(buff);
        return true;
    }
    return false;
}
bool h5helper_v1_0::read(CommonFG* h5obj, const char* name, QStringList& S)
{
    if (!h5exist_ds(h5obj,name)) {
        pushWarning(QString("DataSet '%1' not found in group '%2'")
                .arg(name).arg(groupName(h5obj)));
        return false;
    }
    DataSet ds = h5obj->openDataSet( name );
    H5T_class_t type_class = ds.getTypeClass();
    if (type_class==H5T_STRING)
    {
        // get string type & size
        StrType st = ds.getStrType();
        int ncols = st.getSize();
        /*
         * Get dataspace and allocate memory for read buffer.
         */
        DataSpace space= ds.getSpace();
        hsize_t sz;
        space.getSimpleExtentDims(&sz);
        int nrows = sz;
        QVector<char> rbuff(nrows*ncols,0);

        ds.read(rbuff.data(),st,space);
        for(int i=0; i<nrows; i++)
        {
            QByteArray ba(rbuff.constData() + i*ncols, ncols);
            S << QString(ba);
        }
        return true;
    }
    return false;
}

void h5helper_v1_0::write(CommonFG* h5obj, const char* name, const QDaqVector& v)
{
    hsize_t dims = v.size();
    if (dims<1) dims=1;
    DataSpace space(1,&dims);
    DataSet ds = h5obj->createDataSet(name, PredType::NATIVE_DOUBLE, space);
    if (v.size()) ds.write(v.constData(),PredType::NATIVE_DOUBLE);
}

bool h5helper_v1_0::read(CommonFG* h5obj, const char* name, QDaqVector& value)
{
    if (!h5exist_ds(h5obj,name)) {
        pushWarning(QString("DataSet '%1' not found in group '%2'")
                .arg(name).arg(groupName(h5obj)));
        return false;
    }
    DataSet ds = h5obj->openDataSet(name);
    H5T_class_t ds_type = ds.getTypeClass();
    if (ds_type==H5T_FLOAT)
    {
        DataSpace dspace = ds.getSpace();
        int sz = dspace.getSimpleExtentNpoints();
        value.setCapacity(sz);
        ds.read(value.data(), ds.getDataType());
        return true;
    }
    return false;
}

bool h5helper_v1_0::read(CommonFG* h5obj, const char* name, int& value)
{
    if (!h5exist_ds(h5obj,name)) {
        pushWarning(QString("DataSet '%1' not found in group '%2'")
                .arg(name).arg(groupName(h5obj)));
        return false;
    }
    DataSet ds = h5obj->openDataSet(name);
    H5T_class_t ds_type = ds.getTypeClass();
    if (ds_type==H5T_INTEGER)
    {
        ds.read(&value, ds.getDataType());
        return true;
    }
    return false;
}

bool h5helper_v1_0::read(CommonFG* h5obj, const char* name, double& value)
{
    if (!h5exist_ds(h5obj,name)) {
        pushWarning(QString("DataSet '%1' not found in group '%2'")
                .arg(name).arg(groupName(h5obj)));
        return false;
    }
    DataSet ds = h5obj->openDataSet(name);
    H5T_class_t ds_type = ds.getTypeClass();
    if (ds_type==H5T_FLOAT)
    {
        ds.read(&value, ds.getDataType());
        return true;
    }
    return false;
}

void h5helper_v1_0::write(CommonFG* h5obj, const char* name, const double& value)
{
    DataSpace space(H5S_SCALAR);
    DataSet ds = h5obj->createDataSet(name,PredType::NATIVE_DOUBLE, space);
    ds.write(&value,PredType::NATIVE_DOUBLE);
}

void h5helper_v1_0::write(CommonFG* h5obj, const char* name, const int& value)
{
    DataSpace space(H5S_SCALAR);
    DataSet ds = h5obj->createDataSet(name,PredType::NATIVE_INT, space);
    ds.write(&value,PredType::NATIVE_INT);
}

void h5helper_v1_0::writeProperties(CommonFG* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject)
{
    // get the super-class meta-object
    const QMetaObject* super = metaObject->superClass();

    // if it is null -> we reached the top, i.e. the QObject level -> return
    if (!super) return;

    // if this is the first , write the class name
    if (metaObject==m_object->metaObject())
    {
        write(h5obj,CLASS_ATTR_NAME, QString(metaObject->className()));
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
                write(h5obj,metaProperty.name(),metaEnum.valueToKey(i));
            }
            else {
                int objtype = value.userType();
                if (objtype==QVariant::Bool || objtype==QVariant::Char ||
                    objtype==QVariant::Int || objtype==QVariant::UInt)
                {
                    write(h5obj,metaProperty.name(),value.toInt());
                }
                else if (objtype==QVariant::String)
                {
                    QString S = value.toString();
                    if (S.isEmpty()) S=" ";
                    write(h5obj,metaProperty.name(),S);
                }
                else if (objtype==QVariant::StringList)
                {
                    QStringList S = value.toStringList();
                    if (!S.isEmpty())
                    write(h5obj,metaProperty.name(),S);
                }
                else if (objtype==QVariant::Double) write(h5obj,metaProperty.name(),value.toDouble());
                else if (objtype==qMetaTypeId<QDaqVector>())
                    write(h5obj,metaProperty.name(),value.value<QDaqVector>());
                else if (objtype==qMetaTypeId<QDaqObject*>())
                    write(h5obj,metaProperty.name(),value.value<QDaqObject*>());
            }
        }
    }

    // write the object's dynamic properties
    if (metaObject==m_object->metaObject()) writeDynamicProperties(h5obj,m_object);

}

void h5helper_v1_0::writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object)
{
    if (!m_object->dynamicPropertyNames().isEmpty()) {
        foreach(const QByteArray& ba, m_object->dynamicPropertyNames())
        {
            QVariant v = m_object->property(ba.constData());
            write(h5obj,ba.constData(),v.toString());
        }
    }
}

void h5helper_v1_0::readProperties(CommonFG *h5obj, QDaqObject* obj)
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
                if (read(h5obj,metaProperty.name(),key))
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
                    if (read(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(objtype,&v));
                }
                else if (objtype==QVariant::String)
                {
                    QString v;
                    if (read(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==QVariant::StringList)
                {
                    QStringList v;
                    if (read(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==QVariant::Double)
                {
                    double v;
                    if (read(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==qMetaTypeId<QDaqVector>())
                {
                    QDaqVector v;
                    if (read(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant::fromValue(v));
                }
                else if (objtype==qMetaTypeId<QDaqObject*>())
                {
                    QDaqObject* v;
                    QString path;
                    if (read(h5obj,metaProperty.name(),v,path))
                        metaProperty.write(obj,QVariant::fromValue(v));
                    else if (!path.isEmpty()) deferObjPtrRead(obj,metaProperty.name(),path);
                }
            }
        }
    }

    readDynamicProperties(h5obj,obj);
}

void h5helper_v1_0::readDynamicProperties(CommonFG* h5obj, QDaqObject* m_object)
{
    Q_UNUSED(h5obj);
    Q_UNUSED(m_object);
}

Group h5helper_v1_0::createGroup(CommonFG* loc, const char* name)
{
    return loc->createGroup(name);
}

QByteArrayList h5helper_v1_0::getGroupNames(CommonFG* h5g, bool isRoot)
{
    Q_UNUSED(isRoot);

    QByteArrayList names;

    int n = h5g->getNumObjs();
    for(int i=0; i<n; ++i)
    {
        H5G_obj_t typ = h5g->getObjTypeByIdx(i);
        if (typ==H5G_GROUP)
        {
            QByteArray groupName;
            groupName.fill('\0',256);
            h5g->getObjnameByIdx(i,groupName.data(),256);
            names.push_back(groupName);
        }
    }

    return names;
}



