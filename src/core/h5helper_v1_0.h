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
protected:
    virtual void writeDynamicProperties(const QH5Group& h5obj, const QDaqObject* m_object);
    virtual void readDynamicProperties(const QH5Group& h5obj, QDaqObject* m_object);


public:
    explicit h5helper_v1_0(QDaqH5File* f) : h5helper(QDaqH5File::V_1_0, 1, 0, f)
    {}

    virtual void write(const QH5Group& , const char* , const QDaqObject*) {}
    virtual void write(const QH5Group& , const char* , const QDaqObjectList & ) {}

    virtual void writeProperties(const QH5Group& h5obj, const QDaqObject* m_object, const QMetaObject* metaObject);
    virtual void readProperties(const QH5Group& h5obj, QDaqObject* obj);

    virtual void lockedPropertyList(QStringList S = QStringList()) { Q_UNUSED(S); }

    virtual void connectDeferedPointers() {}
};





#endif // H5HELPER_V1_0_H
