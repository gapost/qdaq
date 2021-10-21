#include "h5helper_v1_1.h"

#include "QDaqObject.h"

#include <QDebug>

#define DYNAMIC_PROPERTY_TAG "DYNAMIC_PROPERTY"

void h5helper_v1_1::write(const QH5Group& h5obj, const char* name, const QDaqObject* obj)
{
    if (!obj) {
        h5obj.write(name,QString("0"));
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
    if (p==top) h5obj.write(name,path);
    else {
        h5obj.write(name,QString("0"));
        pushWarning(QString("QDaqObject* property named: %1 of %2 could not be saved. Pointed object is outside of file scope.").arg(name).arg(path));
    }
}

void h5helper_v1_1::write(const QH5Group& h5obj, const char *name, const QDaqObjectList &objList)
{
    if (objList.isEmpty()) {
        h5obj.write(name,QString("0"));
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
    h5obj.write(name,pathList);
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

void h5helper_v1_1::writeDynamicProperties(const QH5Group& h5obj, const QDaqObject* m_object)
{
    if (m_object->dynamicPropertyNames().isEmpty()) return;


    foreach(const QByteArray& propName, m_object->dynamicPropertyNames())
    {
        if (lockedPropertyList_.contains(propName)) continue;

        QVariant v = m_object->property(propName.constData());

        if (QDaqTypes::isBool(v)) h5obj.write(propName,(int)v.toBool());
        else if (QDaqTypes::isNumeric(v)) h5obj.write(propName,v.toDouble());
        else if (QDaqTypes::isString(v)) h5obj.write(propName,v.toString());
        else if (QDaqTypes::isStringList(v)) h5obj.write(propName,QDaqTypes::toStringList(v));
        else if (QDaqTypes::isVector(v)) h5obj.write(propName,QDaqTypes::toVector(v));
        else continue;

        try {
            QH5Dataset ds = h5obj.openDataset(propName);
            ds.writeAttribute(DYNAMIC_PROPERTY_TAG,1);
        }
        catch(h5exception& e)
        {
            qDebug() << "Exception writing dynamic property " << propName << ", " << e.what();
        }
    }

}

void h5helper_v1_1::readDynamicProperties(const QH5Group& h5g, QDaqObject* m_object)
{
    foreach(const QByteArray& propName, h5g.datasetNames())
    {
        QH5Dataset ds = h5g.openDataset(propName);

        if (ds.hasAttribute(DYNAMIC_PROPERTY_TAG))
        {
            QH5Datatype dtype = ds.datatype();
            QH5Dataspace dspace = ds.dataspace();
            uint sz = 0;
            QVector<quint64> dims = dspace.dimensions();
            if (dims.size()) {
                sz = dims[0];
                for(int i=1; i<dims.size(); i++) sz *= dims[i];
            }

            if (dtype.getClass()==QH5Datatype::INTEGER) { // bool
                if (sz>1) {
                    qDebug() << "Invalid dynamic prop in HDF5"; // only scalar is supported
                } else {
                    int val;
                    ds.read(val);
                    bool b = (bool)val;
                    m_object->setProperty(propName,QVariant::fromValue(b));
                }
            } else if (dtype.getClass()==QH5Datatype::FLOAT) {
                if (sz>1) {
                    QDaqVector val;
                    val.setSize(sz);
                    ds.read(val);
                    m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                } else {
                    double val;
                    ds.read(val);
                    m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                }
            } else if (dtype.getClass()==QH5Datatype::STRING) {
                if (sz>1) {
                    QStringList val;
                    ds.read(val);
                    m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                } else {
                    QString val;
                    ds.read(val);
                    m_object->setProperty(propName.constData(),QVariant::fromValue(val));
                }
            } else {
                qDebug() << "Invalid dynamic prop in HDF5";
            }
        }
    }
}



