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

    f->helper()->lockedPropertyList(columnNames_);

    QDaqObject::writeh5(h5g,f);

    f->helper()->lockedPropertyList();

    if (!(columns() && size())) return;

    for(uint j=0; j<columns(); j++)
        h5g.write(columnNames().at(j).toLatin1(),data_matrix[j]);

}

void QDaqDataBuffer::readh5(const QH5Group &g, QDaqH5File *f)
{
    QDaqObject::readh5(g,f);

    QStringList S;
    if (g.read("columnNames",S) && !S.isEmpty()) {
        setColumnNames(S); // creates data_matrix and dyn. props <-> columns
        for(int j=0; j<S.size(); j++) {
            bool ret = g.read(columnNames().at(j).toLatin1(),data_matrix[j]);
            if (!ret)
                f->helper()->pushWarning(
                            QString("Unable to read QDaqDataBuffer column")
                            );
        }
    }




}


