#include "qdaqh5file.h"

#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqDataBuffer.h"

#include <QDebug>

#include "h5helper_v1_1.h"

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
    H5File *file = 0;

    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        Exception::dontPrint();

        /*
         * Create the named file, truncating the existing one if any,
         * using default create and access property lists.
         */
        file = new H5File( filename.toLatin1(), H5F_ACC_TRUNC );

        // get a last version helper
        newHelper(V_LAST);
        warnings_.clear();

        helper()->write(file, "Timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
        helper()->write(file, "FileType", "QDaq");
        helper()->write(file, "FileVersionMajor", QString::number(helper()->major()));
        helper()->write(file, "FileVersionMinor", QString::number(helper()->minor()));

        top_ = obj;
        writeRecursive(file,obj);

    }  // end of try block

    // catch failure caused by the H5File operations
    catch( FileIException error )
    {
        S = "HDF5 File Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        S = "HDF5 DataSet Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        S = "HDF5 DataSpace Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the Attribute operations
    catch( AttributeIException error )
    {
        S = "HDF5 Attribute Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( DataTypeIException error )
    {
        S = "HDF5 DataType Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( PropListIException error )
    {
        S = "HDF5 PropList Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( GroupIException error )
    {
        S = "HDF5 Group Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    if (file) delete file;

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
    H5File *file = 0;
    QDaqObject* obj(0);

    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        Exception::dontPrint();

        /*
         * Open the named file with read-only access
         */
        file = new H5File( filename.toLatin1(), H5F_ACC_RDONLY );

        newHelper(V_LAST);
        warnings_.clear();

        QString fileType, vMajor, vMinor;

        bool ret = helper()->read(file,"FileType",fileType);
        if (!ret) {
            lastError_ = "Error reading file: 'FileType' field not found";
            return 0;
        }
        if (fileType!="QDaq") {
            lastError_ = "Error reading file: 'FileType' is not 'QDaq'";
            return 0;
        }
        ret = helper()->read(file,"FileVersionMajor",vMajor);
        if (!ret) {
            lastError_ = "Error reading file: 'FileVersionMajor' field not found";
            return 0;
        }
        ret = helper()->read(file,"FileVersionMinor",vMinor);
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
        readRecursive(file,obj);

    }  // end of try block

    // catch failure caused by the H5File operations
    catch( FileIException error )
    {
        S = "HDF5 File Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        S = "HDF5 DataSet Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        S = "HDF5 DataSpace Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the Attribute operations
    catch( AttributeIException error )
    {
        S = "HDF5 Attribute Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( DataTypeIException error )
    {
        S = "HDF5 DataType Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( PropListIException error )
    {
        S = "HDF5 PropList Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    // catch failure caused by the DataType operations
    catch( GroupIException error )
    {
        S = "HDF5 Group Error.";
        S += " In function ";
        S += error.getCFuncName();
        S += ". ";
        S += error.getCDetailMsg();
    }

    if (file) delete file;

    if (!S.isEmpty())
    {
        qDebug() << S;
        lastError_ = S;
    }

    return obj;
}

void QDaqH5File::writeRecursive(CommonFG* h5g, const QDaqObject* obj)
{
    H5::Group objGroup = helper()->createGroup(h5g, obj->objectName().toLatin1().constData());

    obj->writeh5(&objGroup, this);

    foreach(const QDaqObject* ch, obj->children()) writeRecursive(&objGroup, ch);
}

void QDaqH5File::readRecursive(CommonFG* h5g, QDaqObject* &parent_obj)
{
    bool isRoot = parent_obj==0;
    QByteArrayList groups = helper()->getGroupNames(h5g, isRoot);

    foreach(QByteArray groupName, groups) {
        Group g = h5g->openGroup(groupName);
        QString className;
        if (helper()->read(&g,CLASS_ATTR_NAME,className))
        {
            if (className=="QDaqRoot") className = "QDaqObject";
            QDaqObject* obj  = QDaqObject::root()->createObject(groupName,className);
            if (obj)
            {
                if (parent_obj) parent_obj->appendChild(obj);
                else {
                    parent_obj = obj;
                    top_ = obj;
                }

                obj->readh5(&g,this);
                readRecursive(&g, obj);
            }
            else delete obj;
        }
    }

    if (isRoot) helper()->connectDeferedPointers();
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

// Check if a dataset "name" exists in H5 file/group
bool h5helper::h5exist_ds(CommonFG* h5obj, const char* name)
{
    H5O_info_t info;
    return H5Lexists(h5obj->getLocId(),name,0) &&
           H5Oget_info_by_name(h5obj->getLocId(),name,&info,0)>=0 &&
           info.type==H5O_TYPE_DATASET;
}

QString h5helper::groupName(CommonFG* h5obj)
{
    int sz = H5Iget_name(h5obj->getLocId(),0,0);
    if (sz) {
        QByteArray ba(sz+1,char(0));
        H5Iget_name(h5obj->getLocId(),ba.data(),sz+1);
        return QString(ba);
    } else return QString();
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
