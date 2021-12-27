#ifndef QDAQH5FILE_H
#define QDAQH5FILE_H

class h5helper;

#include "QDaqTypes.h"
#include "QDaqObject.h"

#include "qthdf5.h"

#include <string>

template<>
class QH5Datatype::traits<QDaqVector> {
public:

    static int metaTypeId(const QDaqVector &)
    { return qMetaTypeId<double>(); }

    static QH5Dataspace dataspace(const QDaqVector &value)
    { return QVector<quint64>(1,value.size()); }

    static void resize(QDaqVector & v, int n)
    { v.resize(n); }

    static void *ptr(QDaqVector &data) {
        return reinterpret_cast<void *>(data.data());
    }

    static const void *cptr(const QDaqVector &data) {
        return reinterpret_cast<const void *>(data.constData());
    }
};

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
    void writeRecursive(const QH5Group& h5g, const QDaqObject* obj);
    void readRecursive(const QH5Group&  h5g, QDaqObject* &parent_obj);

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

    static bool isQDaq(const QString& fname);

};

class h5helper
{
protected:
    QDaqH5File::Version ver_;
    int major_, minor_;
    QDaqH5File* file_;

    virtual void writeDynamicProperties(const QH5Group&  h5obj, const QDaqObject* m_object) = 0;
    virtual void readDynamicProperties(const QH5Group&  h5obj, QDaqObject* m_object) = 0;

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

    virtual void write(const QH5Group&  h5obj, const char* name, const QDaqObject* obj) = 0;
    virtual void write(const QH5Group&  , const char* , const QDaqObjectList & ) = 0;

    virtual void writeProperties(const QH5Group&  h5obj, const QDaqObject* m_object, const QMetaObject* metaObject) = 0;
    virtual void readProperties(const QH5Group&  h5obj, QDaqObject* obj) = 0;

    virtual void lockedPropertyList(QStringList S = QStringList()) = 0;

    virtual void connectDeferedPointers() = 0;

    void pushWarning(const QString& w) { file_->warnings_.push_back(w); }

    void deferObjPtrRead(QDaqObject* obj, const char* name, const QString& path);
    void deferObjPtrRead(QDaqObject* obj, const char* name, const QStringList& pathList);
};

#endif // QDAQH5FILE_H
