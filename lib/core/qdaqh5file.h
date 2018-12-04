#ifndef QDAQH5FILE_H
#define QDAQH5FILE_H

class h5helper;
class QDaqObject;
class QString;

#include "QDaqTypes.h"

#include <hdf5.h>
#include <H5Cpp.h>

#include <string>

using namespace H5;

class QDaqH5File
{
public:
    enum Version {
        V_INVALID,
        V_1_0,
        V_1_1,
        V_LAST = V_1_1
    };

private:
    h5helper* helper_;
    QString lastError_;

    void getHelper(Version v);
    void writeRecursive(CommonFG* h5g, const QDaqObject* obj);
    void readRecursive(CommonFG* h5g, QDaqObject* &parent_obj);

    static Version toVersion(int mj, int mn)
    {
        if (mj==1 && mn==0) return V_1_0;
        if (mj==1 && mn==1) return V_1_1;
        return V_INVALID;
    }

public:
    QDaqH5File();
    ~QDaqH5File();

    /// Serialize a QDaqObject to a HDF5 file.
    bool h5write(const QDaqObject* obj, const QString& filename);
    /// Load a QDaqObject from a HDF5 file.
    QDaqObject* h5read(const QString& filename);

    h5helper* helper() { return helper_; }

    QString lastError() const { return lastError_; }

};

class h5helper
{
protected:
    QDaqH5File::Version ver_;
    int major_, minor_;

    virtual void writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object) = 0;
    virtual void readDynamicProperties(CommonFG* h5obj, QDaqObject* m_object) = 0;

public:
    h5helper(QDaqH5File::Version v, int mj, int mn) : ver_(v), major_(mj), minor_(mn)
    {}
    virtual ~h5helper()
    {}

    QDaqH5File::Version version() const { return ver_; }

    int major() const { return major_; }
    int minor() const { return minor_; }

    virtual void write(CommonFG* h5obj, const char* name, const int &v) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const double& v) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const QString& S) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const QStringList& S) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const QDaqVector& value) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const QDaqIntVector& value) = 0;
    virtual void write(CommonFG* h5obj, const char* name, const QDaqUintVector& value) = 0;

    virtual bool read(CommonFG* h5obj, const char* name, int& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, double& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QString& str) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QStringList& S) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QDaqVector& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QDaqIntVector& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QDaqUintVector& value) = 0;

    virtual void writeProperties(CommonFG* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject) = 0;
    virtual void readProperties(CommonFG* h5obj, QDaqObject* obj) = 0;

    virtual Group createGroup(CommonFG* loc, const char* name) = 0;

    virtual QByteArrayList getGroupNames(CommonFG* h5obj, bool isRoot = false) = 0;
};

#endif // QDAQH5FILE_H
