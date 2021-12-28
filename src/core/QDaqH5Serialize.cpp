#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqDataBuffer.h"
#include "qdaqh5file.h"

#include <QDebug>

#define CLASS_ATTR_NAME "Class"

void QDaqObject::writeh5(const QH5Group &h5g, QDaqH5File *f) const
{
    f->helper()->writeProperties(h5g,this,metaObject());
}

void QDaqObject::readh5(const QH5Group &g, QDaqH5File *f)
{
    f->helper()->readProperties(g,this);
}

void QDaqDataBuffer::writeh5(const QH5Group &h5g, QDaqH5File *f) const
{
    // add columns to the locked prop list
    // so that they are not automatically written by QDaqObject::writeh5
    f->helper()->lockedPropertyList(columnNames_);

    QDaqObject::writeh5(h5g,f);

    f->helper()->lockedPropertyList();

    // now write the data columns
    for(uint j=0; j<columns(); j++)
        h5g.write(columnNames().at(j).toLatin1(),data_matrix[j]);

}

void QDaqDataBuffer::readh5(const QH5Group &g, QDaqH5File *f)
{
    QDaqObject::readh5(g,f);

    // now read the data columns
    // data_matrix has been created & initialized
    // when reading the columnNames property
    int sz = columnNames().size();
    for(int i=0; i<sz; i++) {
        const char* col_name = columnNames().at(i).toLatin1().constData();
        bool ret = g.read(col_name,data_matrix[i]);
        if (!ret)
            f->helper()->pushWarning(
                        QString("Unable to read QDaqDataBuffer column %1").arg(col_name)
                        );
    }

}


