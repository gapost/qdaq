#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqTypes.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QDebug>
#include <QDateTime>

#include <H5Cpp.h>

#include <string>

using namespace H5;

#define CLASS_ATTR_NAME "Class"

void writeFileAttribute(H5File* h5obj, const char* name, const char* value);
QString readFileAttribute(H5File* h5obj, const char* name);
void writeProperties(H5Object* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject);
bool readStringAttribute(H5Object* h5obj, const char* name, QByteArray& val);
void readProperties(H5Object* h5obj, QDaqObject* obj);


void loadChildren(CommonFG* h5obj, QDaqObject* parent)
{
    int n = h5obj->getNumObjs();
    for(int i=0; i<n; ++i)
    {
        H5G_obj_t typ = h5obj->getObjTypeByIdx(i);
        if (typ==H5G_GROUP)
        {
            QByteArray groupName;
            groupName.fill('\0',256);
            h5obj->getObjnameByIdx(i,groupName.data(),256);
            Group g = h5obj->openGroup(groupName);
            QByteArray className;
            if (readStringAttribute(&g,CLASS_ATTR_NAME,className))
            {
                QDaqObject* obj  = QDaqObject::root()->createObject(groupName,className);
                if (obj && obj->readH5(&g))
                    parent->appendChild(obj);
                else delete obj;
            }
        }
    }
}

void QDaqObject::writeH5(H5File *file, QDaqObject *from, bool recursive)
{
    Group* group;

    if (this==from)
        group = new Group(file->openGroup("/"));
    else {
        /*
         * Create a new group in the file
         */
        QString name = fullName();
        QString fromName = from->fullName();
        name.remove(0,fromName.length());
        name.replace('.','/');

        group = new Group( file->createGroup( name.toLatin1() ));
    }

    writeProperties(group,this,metaObject());

    if (recursive)
        foreach(QDaqObject* obj, children_) obj->writeH5(file, from, recursive);

    delete group;
}
bool QDaqObject::readH5(Group *h5obj)
{
    readProperties(h5obj,this);

    loadChildren(h5obj,this);

    return true;
}

void QDaqRoot::h5write(const QString& filename)
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

		writeFileAttribute(file, "Timestamp", QDateTime::currentDateTime().toString(Qt::ISODate).toLatin1().constData());
        writeFileAttribute(file, "FileType", "RtLab");
        writeFileAttribute(file, "FileVersionMajor", "1");
        writeFileAttribute(file, "FileVersionMinor", "0");

        foreach(QDaqObject* obj, children_) obj->writeH5(file,this);

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
        throwScriptError(S);
        qDebug() << S;
    }
}
void QDaqRoot::h5read(const QString& filename)
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
         * Open the named file with read-only access
         */
        file = new H5File( filename.toLatin1(), H5F_ACC_RDONLY );

        QString value;

        bool ret = readFileAttribute(file,"FileType")==QString("RtLab")
                && readFileAttribute(file,"FileVersionMajor")==QString("1")
                && readFileAttribute(file,"FileVersionMinor")==QString("0");

        if (ret) loadChildren(file,this);
        else
            S = "This is not an RtLab-HDF5 file.";


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
        throwScriptError(S);
        qDebug() << S;
    }
}
void writeStringAttribute(H5Object* h5obj, const char* name, const char* value)
{
	int len = strlen(value);
	if (len==0) return;
	StrType type(PredType::C_S1, len);
	hsize_t dims = 1;
	DataSpace space(1,&dims);
	Attribute* attr = new Attribute( h5obj->createAttribute(name, type, space) );
	attr->write(type, value);
	delete attr;
}
void writeFileAttribute(H5File* h5obj, const char* name, const char* value)
{
	int len = strlen(value);
	if (len==0) return;
	StrType type(PredType::C_S1, len);
	hsize_t dims = 1;
	DataSpace space(1,&dims);
	DataSet* ds = new DataSet( h5obj->createDataSet(name, type, space) );
	ds->write(value,type);
	delete ds;
}
QString readFileAttribute(H5File* h5obj, const char* name)
{
    QString ret;
    DataSet ds = h5obj->openDataSet( name );
    H5T_class_t type_class = ds.getTypeClass();
    if (type_class==H5T_STRING)
    {
        StrType st = ds.getStrType();
        int sz = st.getSize();
        QByteArray buff(sz,'0');
        ds.read(buff.data(),st);
        ret = QString(buff);
    }
    return ret;
}
void writeProperties(H5Object* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject)
{
	// get the super-class meta-object
	const QMetaObject* super = metaObject->superClass();

	// if it is null -> we reached the top, i.e. the QObject level -> return
	if (!super) return;

	// if this is the first , write the class name
	if (metaObject==m_object->metaObject())
	{
        writeStringAttribute(h5obj,CLASS_ATTR_NAME, metaObject->className());
	}

	// first write the properties of the super-class (this produces all properties up to the current object)
	writeProperties(h5obj, m_object, super);

	// write this class properties
	for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) 
	{
		QMetaProperty metaProperty = metaObject->property(idx);
		if (metaProperty.isReadable())
		{
			QVariant value = metaProperty.read(m_object);
			if (metaProperty.isEnumType())
			{
				QMetaEnum metaEnum = metaProperty.enumerator();
				int i = *reinterpret_cast<const int *>(value.constData());
				writeStringAttribute(h5obj,metaProperty.name(),metaEnum.valueToKey(i));
			}
			else {
				int objtype = value.userType();
				if (objtype==QVariant::Bool || objtype==QVariant::Char ||
					objtype==QVariant::Int || objtype==QVariant::UInt)
				{
					DataSpace space(H5S_SCALAR);
					Attribute* attr = new Attribute(
						h5obj->createAttribute(metaProperty.name(),
						PredType::NATIVE_INT, space)
						);
					int i = value.toInt();
					attr->write(PredType::NATIVE_INT,&i);
					delete attr;
				}
				else if (objtype==QVariant::String)
				{
					QString S = value.toString();
					if (S.isEmpty()) S=" ";
					writeStringAttribute(h5obj,metaProperty.name(),S.toLatin1().constData());
				}
				else if (objtype==QVariant::Double)
				{
					DataSpace space(H5S_SCALAR);
					Attribute* attr = new Attribute(
						h5obj->createAttribute(metaProperty.name(),
						PredType::NATIVE_DOUBLE, space)
						);
					double d = value.toDouble();
					attr->write(PredType::NATIVE_DOUBLE,&d);
					delete attr;
				}
                else if (objtype==qMetaTypeId<QDaqIntVector>())
				{
                    QDaqIntVector v = value.value<QDaqIntVector>();
					hsize_t sz = v.size();
					if (sz<1) sz=1;
					ArrayType type(PredType::NATIVE_INT, 1, &sz);
					hsize_t dims = 1;
					DataSpace space(1,&dims);
					Attribute* attr = new Attribute(
						h5obj->createAttribute(metaProperty.name(),
						type, space)
						);
					if (v.size())
						attr->write(type, v.constData());
					delete attr;
				}
                else if (objtype==qMetaTypeId<QDaqUintVector>())
				{
                    QDaqUintVector v = value.value<QDaqUintVector>();
					hsize_t sz = v.size();
					if (sz<1) sz=1;
					ArrayType type(PredType::NATIVE_UINT, 1, &sz);
					hsize_t dims = 1;
					DataSpace space(1,&dims);
					Attribute* attr = new Attribute(
						h5obj->createAttribute(metaProperty.name(),
						type, space)
						);
					if (v.size())
						attr->write(type, v.constData());
					delete attr;
				}
                else if (objtype==qMetaTypeId<QDaqDoubleVector>())
				{
                    QDaqDoubleVector v = value.value<QDaqDoubleVector>();
					hsize_t sz = v.size();
					if (sz<1) sz=1;
					ArrayType type(PredType::NATIVE_DOUBLE, 1, &sz);
					hsize_t dims = 1;
					DataSpace space(1,&dims);
					Attribute* attr = new Attribute(
						h5obj->createAttribute(metaProperty.name(),
						type, space)
						);
					if (v.size())
						attr->write(type, v.constData());
					delete attr;
				}
			}
		}
	}
}
bool readStringAttribute(H5Object* h5obj, const char* name, QByteArray& val)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_STRING)
    {
        StrType strType = attr.getStrType();
        int sz = strType.getSize();
        val.fill('0',sz);
        attr.read(strType,val.data());
        return true;
    }
    return false;
}
bool readIntAttribute(H5Object* h5obj, const char* name, int &v)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_INTEGER)
    {
        IntType intType = attr.getIntType();
        attr.read(intType,&v);
        return true;
    }
    return false;
}
bool readDoubleAttribute(H5Object* h5obj, const char* name, double &v)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_FLOAT)
    {
        attr.read(PredType::NATIVE_DOUBLE,&v);
        return true;
    }
    return false;
}
bool readQDaqIntVectorAttribute(H5Object* h5obj, const char* name, QDaqIntVector &v)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_INTEGER)
    {
        ArrayType arrayType = attr.getArrayType();
        int sz = arrayType.getSize();
        v.fill(0,sz);
        attr.read(arrayType,v.data());
        return true;
    }
    return false;
}
bool readQDaqUintVectorAttribute(H5Object* h5obj, const char* name, QDaqUintVector &v)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_INTEGER)
    {
        ArrayType arrayType = attr.getArrayType();
        int sz = arrayType.getSize();
        v.fill(0,sz);
        attr.read(arrayType,v.data());
        return true;
    }
    return false;
}
bool readQDaqDoubleVectorAttribute(H5Object* h5obj, const char* name, QDaqDoubleVector &v)
{
    Attribute attr = h5obj->openAttribute(name);
    H5T_class_t typ = attr.getTypeClass();
    if (typ==H5T_FLOAT)
    {
        ArrayType arrayType = attr.getArrayType();
        int sz = arrayType.getSize();
        v.fill(0,sz);
        attr.read(arrayType,v.data());
        return true;
    }
    return false;
}
void readProperties(H5Object* h5obj, QDaqObject* obj)
{
    // get the meta-object
    const QMetaObject* metaObject = obj->metaObject();

    // the "class" and "objectName" properties have been read when the obj was constructed
    // start at index = 2
    for (int idx = 2; idx < metaObject->propertyCount(); idx++)
    {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isWritable())
        {
            if (metaProperty.isEnumType())
            {
                QMetaEnum metaEnum = metaProperty.enumerator();
                QByteArray key;
                if (readStringAttribute(h5obj,metaProperty.name(),key))
                {
                    int v = metaEnum.keyToValue(key);
                    if (v>=0)
                        metaProperty.write(obj,QVariant(metaEnum.keyToValue(key)));
                }
            }
            else {
                int objtype = metaProperty.type();
                if (objtype==QVariant::Bool || objtype==QVariant::Char ||
                    objtype==QVariant::Int || objtype==QVariant::UInt)
                {
                    int v;
                    if (readIntAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(objtype,&v));
                }
                else if (objtype==QVariant::String)
                {
                    QByteArray v;
                    if (readStringAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(QString(v)));
                }
                else if (objtype==QVariant::Double)
                {
                    double v;
                    if (readDoubleAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant(v));
                }
                else if (objtype==qMetaTypeId<QDaqIntVector>())
                {
                    QDaqIntVector v;
                    if (readQDaqIntVectorAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant::fromValue(v));
                }
                else if (objtype==qMetaTypeId<QDaqUintVector>())
                {
                    QDaqUintVector v;
                    if (readQDaqUintVectorAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant::fromValue(v));
                }
                else if (objtype==qMetaTypeId<QDaqDoubleVector>())
                {
                    QDaqDoubleVector v;
                    if (readQDaqDoubleVectorAttribute(h5obj,metaProperty.name(),v))
                        metaProperty.write(obj,QVariant::fromValue(v));
                }
            }
        }
    }
}


