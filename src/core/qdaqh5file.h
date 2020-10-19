#ifndef QDAQH5FILE_H
#define QDAQH5FILE_H

class h5helper;

#include "QDaqTypes.h"
#include "QDaqObject.h"

#include <string>

namespace H5
{
class CommonFG;
class Group;
}

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
    friend class h5helper;
    h5helper* helper_;
    QString lastError_;
    QStringList warnings_;
    const QDaqObject* top_;

    void newHelper(Version v);
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
    const QStringList& warnings() const { return warnings_; }

    const QDaqObject* getTopObject() const { return top_; }

};

class h5helper
{
protected:
    QDaqH5File::Version ver_;
    int major_, minor_;
    QDaqH5File* file_;

    virtual void writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object) = 0;
    virtual void readDynamicProperties(CommonFG* h5obj, QDaqObject* m_object) = 0;

    struct deferedPtrData {
        QDaqObject* obj;
        const char* propName;
        QStringList pathList;
        bool isList;
        deferedPtrData(QDaqObject* o = 0, const char* n = 0, const QString& p = QString()) :
            obj(o), propName(n), isList(false)
        {
            if (!p.isEmpty()) pathList << p;
        }
        deferedPtrData(QDaqObject* o, const char* n, const QStringList& p) :
            obj(o), propName(n), pathList(p), isList(true)
        {}
    };

    QList<deferedPtrData> deferedPtrs;

public:
    h5helper(QDaqH5File::Version v, int mj, int mn, QDaqH5File* f) :
        ver_(v), major_(mj), minor_(mn), file_(f)
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
    virtual void write(CommonFG* h5obj, const char* name, const QDaqObject* obj) = 0;
    virtual void write(CommonFG* , const char* , const QDaqObjectList & ) = 0;

    virtual bool read(CommonFG* h5obj, const char* name, int& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, double& value) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QString& str) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QStringList& S) = 0;
    virtual bool read(CommonFG* h5obj, const char* name, QDaqVector& value) = 0;

    virtual void writeProperties(CommonFG* h5obj, const QDaqObject* m_object, const QMetaObject* metaObject) = 0;
    virtual void readProperties(CommonFG* h5obj, QDaqObject* obj) = 0;

    virtual void lockedPropertyList(QStringList S = QStringList()) = 0;

    virtual void connectDeferedPointers() = 0;

    virtual Group createGroup(CommonFG* loc, const char* name) = 0;

    virtual QByteArrayList getGroupNames(CommonFG* h5obj, bool isRoot = false) = 0;

    // Check if a dataset "name" exists in H5 file/group
    bool h5exist_ds(CommonFG* h5obj, const char* name);
    // Get group name from obj pointer
    QString groupName(CommonFG* h5obj);

    void pushWarning(const QString& w) { file_->warnings_.push_back(w); }

    void deferObjPtrRead(QDaqObject* obj, const char* name, const QString& path);
    void deferObjPtrRead(QDaqObject* obj, const char* name, const QStringList& pathList);
};

#endif // QDAQH5FILE_H
