#include "h5helper_v1_1.h"

#include "QDaqObject.h"

#include <QDebug>

#define DYNAMIC_PROPERTY_TAG "DYNAMIC_PROPERTY"

Group h5helper_v1_1::createGroup(CommonFG* loc, const char* name)
{
    hid_t group_creation_plist;
    group_creation_plist = H5Pcreate(H5P_GROUP_CREATE);
    herr_t status = H5Pset_link_creation_order(group_creation_plist,
                                     H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);

    if (status<0) throw PropListIException("H5Pset_link_creation_order");

    hid_t group_id;
    group_id = H5Gcreate(loc->getLocId(),
                         name,
                         H5P_DEFAULT,
                         group_creation_plist,
                         H5P_DEFAULT);

    Group h5g(group_id);

    H5Pclose(group_creation_plist);
   //5Gclose(group_id);

    return h5g;
}

QByteArrayList h5helper_v1_1::getGroupNames(CommonFG* h5g, bool isRoot)
{
    QByteArrayList names;

    int n = h5g->getNumObjs();

    if (isRoot) {
        for(int i=0; i<n; ++i) {
            H5G_obj_t typ = h5g->getObjTypeByIdx(i);
            if (typ==H5G_GROUP)
            {
                QByteArray groupName;
                groupName.fill('\0',256);
                h5g->getObjnameByIdx(i,groupName.data(),256);
                names.push_back(groupName);
            }
        }
    } else {
        for(int i=0; i<n; ++i) {
            QByteArray groupName;
            groupName.fill('\0',256);

            herr_t ret = H5Lget_name_by_idx(h5g->getLocId(), ".", H5_INDEX_CRT_ORDER, H5_ITER_INC,
                                            i, groupName.data(), 256, 0);
            if (ret<0) break;
            H5O_info_t object_info;
            ret = H5Oget_info_by_name(h5g->getLocId(), groupName, &object_info, 0);
            if (object_info.type == H5O_TYPE_GROUP) names.push_back(groupName);

        }
    }

    return names;
}

bool isTypeNumeric(int id)
{
    return id==QMetaType::Bool || id==QMetaType::Int || id==QMetaType::UInt ||
            id==QMetaType::Long || id==QMetaType::ULong || id==QMetaType::LongLong ||
            id==QMetaType::ULongLong || id==QMetaType::Short || id==QMetaType::UShort ||
            id==QMetaType::Float || id==QMetaType::Double;
}

bool isTypeString(int id)
{
    return id==QMetaType::QString || id==QMetaType::QByteArray ||
            id==QMetaType::QChar || id==QMetaType::Char || id==QMetaType::SChar ||
            id==QMetaType::UChar;
}

bool isTypeNumeric(const QVariantList& L)
{
    foreach(const QVariant& v, L) {
        if (!isTypeNumeric(v.type())) return false;
    }
    return true;
}

void h5helper_v1_1::writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object)
{
    if (m_object->dynamicPropertyNames().isEmpty()) return;


    foreach(const QByteArray& propName, m_object->dynamicPropertyNames())
    {
        QVariant v = m_object->property(propName.constData());
        int id = v.type();

        if (isTypeNumeric(id))
        {
            write(h5obj,propName.constData(),v.toDouble());
        } else if (isTypeString(id))
        {
            write(h5obj,propName.constData(),v.toString());
        }
        else if (id==QMetaType::QVariantList) {
            QVariantList vl = v.value<QVariantList>();
            if (isTypeNumeric(vl)) {
                int n = vl.length();
                QDaqVector x(n);
                for(int i=0; i<n; ++i) x[i] = vl.at(i).toDouble();
                write(h5obj,propName.constData(),x);
            } else
                write(h5obj,propName.constData(),v.toStringList());
        }
        else if (id==QMetaType::QStringList)
            write(h5obj,propName.constData(),v.toStringList());
        else if (id==QMetaType::QByteArrayList)
        {
            QStringList L;
            QByteArrayList balist = v.value<QByteArrayList>();
            foreach(QByteArray ba, balist) L << QString(ba);
            write(h5obj,propName.constData(),L);
        }
        else if (id==qMetaTypeId<QDaqVector>())
            write(h5obj,propName.constData(),v.value<QDaqVector>());
        else if (id==qMetaTypeId<QDaqIntVector>())
            write(h5obj,propName.constData(),v.value<QDaqIntVector>());
        else if (id==qMetaTypeId<QDaqUintVector>())
            write(h5obj,propName.constData(),v.value<QDaqUintVector>());

        try {
            DataSet ds = h5obj->openDataSet(propName.constData());
            DataSpace dspace(H5S_SCALAR);
            Attribute attr = ds.createAttribute(DYNAMIC_PROPERTY_TAG,PredType::NATIVE_INT,dspace);
            int va = 1;
            attr.write(PredType::NATIVE_INT,&va);
        }
        catch(FileIException& e)
        {
            qDebug() << "File exception writing dynamic property " << propName;

        }
        catch(GroupIException& e)
        {
            qDebug() << "Group exception writing dynamic property " << propName;
        }
        catch(AttributeIException& e)
        {
            qDebug() << "Attribute exception writing dynamic property " << propName;
        }


    }

}

void h5helper_v1_1::readDynamicProperties(CommonFG* h5g, QDaqObject* m_object)
{
    int n = h5g->getNumObjs();

    for(int i=0; i<n; ++i) {
        H5G_obj_t typ = h5g->getObjTypeByIdx(i);
        if (typ==H5G_DATASET)
        {
            QByteArray propName;
            propName.fill('\0',256);
            h5g->getObjnameByIdx(i,propName.data(),256);

            DataSet ds = h5g->openDataSet(propName.constData());

            if (ds.attrExists(DYNAMIC_PROPERTY_TAG))
            {
                H5T_class_t type_class = ds.getTypeClass();
                DataSpace dspace = ds.getSpace();

                if (type_class==H5T_INTEGER) {
                    int sz = dspace.getSimpleExtentNpoints();
                    if (sz>1) {
                        QDaqIntVector val;
                        val.fill(0,sz);
                        ds.read(val.data(), ds.getDataType());
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    } else {
                        int val;
                        ds.read(&val, ds.getDataType());
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    }
                } else if (type_class==H5T_FLOAT) {
                    int sz = dspace.getSimpleExtentNpoints();
                    if (sz>1) {
                        QDaqVector val;
                        val.fill(0.,sz);
                        ds.read(val.data(), ds.getDataType());
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    } else {
                        double val;
                        ds.read(&val, ds.getDataType());
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    }
                } else if (type_class==H5T_STRING) {
                    // get string type & size
                    StrType st = ds.getStrType();
                    int ncols = st.getSize();
                    hsize_t sz;
                    dspace.getSimpleExtentDims(&sz);
                    int nrows = sz;
                    QByteArray buff(nrows*ncols,'0');
                    ds.read(buff.data(),st,dspace);
                    if (nrows>1) {
                        QStringList val;
                        for(int i=0; i<nrows; i++)
                        {
                            QByteArray ba(buff.constData() + i*ncols, ncols);
                            val << QString(ba);
                        }
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    } else {
                        QString val(buff);
                        m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                    }
                } else {
                    qDebug() << "Invalid dynamic prop in HDF5";
                }
            }
        }
    }
}


