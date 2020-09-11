#ifndef H5HELPER_V1_1_H
#define H5HELPER_V1_1_H

#include "h5helper_v1_0.h"


class h5helper_v1_1 : public h5helper_v1_0
{
protected:
    virtual void writeDynamicProperties(CommonFG* h5obj, const QDaqObject* m_object);
    virtual void readDynamicProperties(CommonFG* h5obj, QDaqObject* m_object);

    QStringList lockedPropertyList_;

public:
    h5helper_v1_1(QDaqH5File* f) : h5helper_v1_0(f)
    {
        ver_ = QDaqH5File::V_1_1;
        major_ = 1;
        minor_ = 1;
    }

    virtual void write(CommonFG* h5obj, const char* name, const QDaqObject* obj);
    virtual void write(CommonFG* h5obj, const char* name, const QDaqObjectList & objList);

    virtual void lockedPropertyList(QStringList S = QStringList())
    { lockedPropertyList_ = S; }

    virtual void connectDeferedPointers();

    virtual Group createGroup(CommonFG* loc, const char* name);

    virtual QByteArrayList getGroupNames(CommonFG* h5obj, bool isRoot = false);
};

#endif // H5HELPER_V1_1_H
