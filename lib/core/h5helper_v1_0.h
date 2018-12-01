#ifndef H5HELPER_V1_0_H
#define H5HELPER_V1_0_H

#include "qdaqh5file.h"


/* Rules for H5 serialization - v1.0
 * - Objects are written as H5 groups
 * - Object properties are written as datasets of the group
 * - Base class QDaqObject::writeH5/readH5 handle all QDaq property types
 * - Other object data (like in QDaqDataBuffer) are also datasets
 * - need to override the basic writeH5/readH5 to handle the object special data reading/writing
 * - Root object -> write as any other obj / read as a plain QDaqObj
 */

class h5helper_v1_0 : public h5helper
{
public:
    h5helper_v1_0() : h5helper(QDaqH5File::V_1_0)
    {}
    virtual void write(CommonFG* h5obj, const char* name, const int &v);
    virtual void write(CommonFG* h5obj, const char* name, const double& v);
    virtual void write(CommonFG* h5obj, const char* name, const QString& S);
    virtual void write(CommonFG* h5obj, const char* name, const QStringList& S);
    virtual void write(CommonFG* h5obj, const char* name, const QDaqVector &v);
    virtual void write(CommonFG* h5obj, const char* name, const QDaqIntVector& v);
    virtual void write(CommonFG* h5obj, const char* name, const QDaqUintVector& v);

    virtual bool read(CommonFG* h5obj, const char* name, int& value);
    virtual bool read(CommonFG* h5obj, const char* name, double& value);
    virtual bool read(CommonFG* h5obj, const char* name, QString& str);
    virtual bool read(CommonFG* h5obj, const char* name, QStringList& S);
    virtual bool read(CommonFG* h5obj, const char* name, QDaqVector& value);
    virtual bool read(CommonFG* h5obj, const char* name, QDaqIntVector& value);
    virtual bool read(CommonFG* h5obj, const char* name, QDaqUintVector& value);

    virtual void writeProperties(CommonFG* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject);
    virtual void readProperties(CommonFG* h5obj, QDaqObject* obj);

    virtual Group createGroup(CommonFG* loc, const char* name);
};





#endif // H5HELPER_V1_0_H
