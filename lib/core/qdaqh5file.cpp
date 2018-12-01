#include "qdaqh5file.h"

#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqDataBuffer.h"

#include <QDebug>

#include "h5helper_v1_0.h"

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
        getHelper(V_LAST);

        helper()->write(file, "Timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
        helper()->write(file, "FileType", "QDaq");
        helper()->write(file, "FileVersionMajor", "1");
        helper()->write(file, "FileVersionMinor", "0");

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

    if (file) delete file;

    if (!S.isEmpty())
    {
        obj->throwScriptError(S);
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

        getHelper(V_LAST);

        QString fileType, vMajor, vMinor;

        bool ret = helper()->read(file,"FileType",fileType) &&
                helper()->read(file,"FileVersionMajor",vMajor) &&
                helper()->read(file,"FileVersionMinor",vMinor) &&
                fileType==QString("QDaq")
                && vMajor==QString("1")
                && vMinor==QString("0");

        if (ret) readRecursive(file,obj);


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

    if (file) delete file;

    if (!S.isEmpty())
    {
        //obj->throwScriptError(S);
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
    int n = h5g->getNumObjs();

    for(int i=0; i<n; ++i)
    {
        H5G_obj_t typ = h5g->getObjTypeByIdx(i);
        if (typ==H5G_GROUP)
        {
            QByteArray groupName;
            groupName.fill('\0',256);
            h5g->getObjnameByIdx(i,groupName.data(),256);
            Group g = h5g->openGroup(groupName);
            QString className;
            if (helper()->read(&g,CLASS_ATTR_NAME,className))
            {
                if (className=="QDaqRoot") className = "QDaqObject";
                QDaqObject* obj  = QDaqObject::root()->createObject(groupName,className);
                if (obj)
                {
                    obj->setObjectName(groupName);
                    obj->readh5(&g,this);
                    readRecursive(&g, obj);
                    if (parent_obj) parent_obj->appendChild(obj);
                    else {
                        parent_obj = obj;
                        return;
                    }
                }
                else delete obj;
            }
        }
    }
}

void QDaqH5File::getHelper(Version v)
{
    Q_UNUSED(v);
    if (helper_) return;
    else helper_ = new h5helper_v1_0;
}
