#include "qdaqh5file.h"

#include <hdf5.h>
#include <H5Cpp.h>

#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqDataBuffer.h"

#include <QMetaType>
#include <QDebug>

#include <stdexcept>

#include "h5helper_v1_1.h"

#include "QDaqJob.h"

#define CLASS_ATTR_NAME "Class"

QDaqH5File::QDaqH5File() : helper_(0)
{

}

QDaqH5File::~QDaqH5File()
{
    if (helper_) delete helper_;
}

bool QDaqH5File::h5write(const QDaqObject* obj, const QString& filename)
{
    QString S;

    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
        QH5File h5f(filename);

        /*
         * Create the named file, truncating the existing one if any,
         * using default create and access property lists.
         */
        if (!h5f.open(QIODevice::Truncate)) {
            // lastError_ =
            return false;
        }

        // get a last version helper
        newHelper(V_LAST);
        warnings_.clear();

        QH5Group root = h5f.root();
        root.write("Timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
        root.write("FileType", QString("QDaq"));
        root.write("FileVersionMajor", QString::number(helper()->major()));
        root.write("FileVersionMinor", QString::number(helper()->minor()));

        top_ = obj;
        writeRecursive(root,obj);

    }  // end of try block

    // catch failure caused by the H5File operations
    catch( h5exception& error )
    {
        S = "HDF5 File Write Error. ";
        S += error.what();
    }

    if (!S.isEmpty())
    {
        qDebug() << S;
        lastError_ = S;
        return false;
    }

    return true;
}

QDaqObject *QDaqH5File::h5read(const QString& filename)
{
    QString S;
    QDaqObject* obj(0);

    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
        QH5File h5f(filename);

        /*
         * Open the named file with read-only access
         */
        if (!h5f.open(QIODevice::ReadOnly)) {
            // lastError_ =
            return obj;
        }

        newHelper(V_LAST);
        warnings_.clear();

        QH5Group root = h5f.root();
        QString fileType, vMajor, vMinor;

        bool ret = root.read("FileType",fileType);
        if (!ret) {
            lastError_ = "Error reading file: 'FileType' field not found";
            return 0;
        }
        if (fileType!="QDaq") {
            lastError_ = "Error reading file: 'FileType' is not 'QDaq'";
            return 0;
        }
        ret = root.read("FileVersionMajor",vMajor);
        if (!ret) {
            lastError_ = "Error reading file: 'FileVersionMajor' field not found";
            return 0;
        }
        ret = root.read("FileVersionMinor",vMinor);
        if (!ret) {
            lastError_ = "Error reading file: 'FileVersionMinor' field not found";
            return 0;
        }
        Version fileVersion = toVersion(vMajor.toInt(),vMinor.toInt());
        if (fileVersion==V_INVALID) {
            lastError_ = QString("Error reading file: Not a valid version, vMajor = %1, vMinor = %2").arg(vMajor).arg(vMinor);
            return 0;
        }

        newHelper(fileVersion);
        readRecursive(root,obj);

    }  // end of try block

    // catch failure caused by the H5File operations
    catch( h5exception& error )
    {
        S = "HDF5 File Read Error. ";
        S += error.what();
    }

    if (!S.isEmpty())
    {
        qDebug() << S;
        lastError_ = S;
    }

    return obj;
}

bool QDaqH5File::isQDaq(const QString &fname)
{
    if (!QH5File::isHDF5(fname)) return false;
    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
        QH5File h5f(fname);

        /*
         * Open the named file with read-only access
         */
        if (!h5f.open(QIODevice::ReadOnly)) {
            // lastError_ =
            return false;
        }

        QH5Group root = h5f.root();
        QString fileType, vMajor, vMinor;
        bool ret;

        ret = root.read("FileType",fileType);
        if (!ret || fileType!="QDaq") return false;

        if (!root.read("FileVersionMajor",vMajor) || !root.read("FileVersionMinor",vMinor)) return false;

        Version fileVersion = toVersion(vMajor.toInt(),vMinor.toInt());
        if (fileVersion==V_INVALID) return false;

        return true;

    }  // end of try block

    // catch failure caused by the H5File operations
    catch( h5exception& error )
    {
        return false;
    }

}

void QDaqH5File::writeRecursive(const QH5Group &h5g, const QDaqObject* obj)
{
    QH5Group objGroup = h5g.createGroup(obj->objectName().toLatin1(),true);

    obj->writeh5(objGroup, this);

    foreach(const QDaqObject* ch, obj->children()) writeRecursive(objGroup, ch);
}

void QDaqH5File::readRecursive(const QH5Group &h5g, QDaqObject* &parent_obj)
{
    QByteArrayList groups = h5g.groupNames(true);

    foreach(QByteArray groupName, groups) {
        QH5Group g = h5g.openGroup(groupName);
        QString className;
        if (g.read(CLASS_ATTR_NAME,className))
        {
            if (className=="QDaqRoot") className = "QDaqObject";
            className += "*";
            int id = QMetaType::type(className.toLatin1());
            if (id != 0) { // TODO: report to user if the type is not found
                const QMetaObject * metaObj = QMetaType::metaObjectForType(id);
                QDaqObject* obj = (QDaqObject*) metaObj->newInstance(Q_ARG(QString,groupName));
                if (obj)
                {
                    if (parent_obj) parent_obj->appendChild(obj);
                    else {
                        parent_obj = obj;
                        top_ = obj;
                    }

                    obj->readh5(g,this);
                    readRecursive(g, obj);
                }
                else delete obj;
            } else {
                QString S = QString("Unknown class %1 for object %2").arg(className).arg(QString(groupName));
                throw h5exception(S.toLatin1());
            }
        }
    }

    // if reading the root
    if (parent_obj==0) helper()->connectDeferedPointers();
}

void QDaqH5File::newHelper(Version v)
{
    if (helper_) {
        delete helper_;
        helper_ = 0;
    }

    switch (v) {
    case V_1_0:
        helper_ = new h5helper_v1_0(this); return;
    case V_1_1:
        helper_ = new h5helper_v1_1(this); return;
    default:
        helper_ = new h5helper_v1_1(this); return;
    }

}

void h5helper::deferObjPtrRead(QDaqObject *obj, const char *name, const QString &path)
{
    deferedPtrData d(obj,name,path);
    deferedPtrs << d;
}

void h5helper::deferObjPtrRead(QDaqObject *obj, const char *name, const QStringList &paths)
{
    deferedPtrData d(obj,name,paths);
    deferedPtrs << d;
}
