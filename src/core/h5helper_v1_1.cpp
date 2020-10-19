#include "h5helper_v1_1.h"

#include <hdf5.h>
#include <H5Cpp.h>

#include "QDaqObject.h"

#include <QDebug>

#define DYNAMIC_PROPERTY_TAG "DYNAMIC_PROPERTY"

void h5helper_v1_1::write(CommonFG* h5obj, const char* name, const QDaqObject* obj)
{
    if (!obj) {
        h5helper_v1_0::write(h5obj,name,QString("0"));
        return;
    }

    QString path(obj->objectName());
    const QDaqObject* top = file_->getTopObject();
    const QDaqObject* p = obj;
    const QDaqObject* r = (QDaqObject*)QDaqObject::root();
    while(p && p!=top && p!=r) {
        p = p->parent();
        if (p) {
            path.push_front('.');
            path.push_front(p->objectName());
        }
    }
    if (p==top) h5helper_v1_0::write(h5obj,name,path);
    else {
        h5helper_v1_0::write(h5obj,name,QString("0"));
        pushWarning(QString("QDaqObject* property named: %1 of %2 could not be saved. Pointed object is outside of file scope.").arg(name).arg(path));
    }
}

void h5helper_v1_1::write(CommonFG *h5obj, const char *name, const QDaqObjectList &objList)
{
    if (objList.isEmpty()) {
        h5helper_v1_0::write(h5obj,name,QString("0"));
        return;
    }

    const QDaqObject* top = file_->getTopObject();
    const QDaqObject* r = (QDaqObject*)QDaqObject::root();
    QStringList pathList;

    foreach(const QDaqObject* p, objList)
    {
        QString path(p->objectName());
        while(p && p!=top && p!=r) {
            p = p->parent();
            if (p) {
                path.push_front('.');
                path.push_front(p->objectName());
            }
        }
        if (p==top) pathList << path;
        else {
            pathList << QString("0");
            pushWarning(QString("A ptr in QDaqObjectList property named: %1 of %2 could not be saved. Pointed object is outside of file scope.").arg(name).arg(path));
        }
    }
    h5helper_v1_0::write(h5obj,name,pathList);
}

void h5helper_v1_1::connectDeferedPointers()
{
    foreach(const deferedPtrData& d, deferedPtrs) {

        QDaqObjectList objList;

        foreach(const QString& path, d.pathList) {

            if (path.isEmpty() || path=="0") {
                objList << (QDaqObject*)0;
                continue;
            }

            QStringList splitPath = path.split(QChar('.'));

            QString objName = splitPath.front(); splitPath.pop_front();
            QDaqObject* p = const_cast<QDaqObject*>(file_->getTopObject());
            if (objName != p->objectName()) {
                pushWarning(QString("Error reading %1.%2 from file: Path %3 not found.")
                            .arg(d.obj->objectName())
                            .arg(d.propName)
                            .arg(path));
                objList << (QDaqObject*)0;
                continue;
            }

            do
            {
                objName = splitPath.front();
                splitPath.pop_front();
                p = p->findChild(objName);
            }
            while (!splitPath.isEmpty() && p);

            if (p) objList << p;
            else {
                pushWarning(QString("Error reading %1.%2 from file: Unknown error.")
                             .arg(d.obj->objectName())
                             .arg(d.propName));
                objList << (QDaqObject*)0;
            }

        }

        if (d.isList) {
            bool noNulls = true;
            foreach(const QDaqObject* p, objList) {
                if (!p) { noNulls = false; break; }
            }
            if (noNulls) d.obj->setProperty(d.propName,QVariant::fromValue(objList));
        } else {
            QDaqObject* p = 0;
            if (!objList.isEmpty()) p = objList.at(0);
            d.obj->setProperty(d.propName,QVariant::fromValue(p));
        }
    }
}


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

/*
 * Dynamic Property Read/Write Conversion Rules
 *
 * QDaq        <->  HDF5
 * bool             int
 * number           float
 * string           string
 * stringlist       string 2D
 * vector           array of float
 *
 */

void h5helper_v1_1::writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object)
{
    if (m_object->dynamicPropertyNames().isEmpty()) return;


    foreach(const QByteArray& propName, m_object->dynamicPropertyNames())
    {
        if (lockedPropertyList_.contains(propName)) continue;

        QVariant v = m_object->property(propName.constData());

        if (QDaqTypes::isBool(v)) h5helper_v1_0::write(h5obj,propName.constData(),(int)v.toBool());
        else if (QDaqTypes::isNumeric(v)) h5helper_v1_0::write(h5obj,propName.constData(),v.toDouble());
        else if (QDaqTypes::isString(v)) h5helper_v1_0::write(h5obj,propName.constData(),v.toString());
        else if (QDaqTypes::isStringList(v)) h5helper_v1_0::write(h5obj,propName.constData(),QDaqTypes::toStringList(v));
        else if (QDaqTypes::isVector(v)) h5helper_v1_0::write(h5obj,propName.constData(),QDaqTypes::toVector(v));
        else continue;

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

                if (type_class==H5T_INTEGER) { // bool
                    int sz = dspace.getSimpleExtentNpoints();
                    if (sz>1) {
                        qDebug() << "Invalid dynamic prop in HDF5"; // only scalar is supported
                    } else {
                        int val;
                        ds.read(&val, ds.getDataType());
                        bool b = (bool)val;
                        m_object->setProperty(propName.constData(),QVariant::fromValue(b));
                    }
                } else if (type_class==H5T_FLOAT) {
                    int sz = dspace.getSimpleExtentNpoints();
                    if (sz>1) {
                        QDaqVector val;
                        val.setSize(sz);
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


