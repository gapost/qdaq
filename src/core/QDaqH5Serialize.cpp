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
    if (g.read("columnNames",S)) columnNames_ = S;

    int ncols = columnNames_.size();
    if (!ncols) return;
    uint cap_ = capacity();
    data_matrix = matrix_t(ncols);
    // restore the capacity && type
    for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].setCircular(circular_);
    QDaqVector rbuff(cap_);
    for(int j=0; j<ncols; j++)
    {
        g.read(columnNames().at(j).toLatin1(),rbuff);
        data_matrix[j] = rbuff;
    }
    for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].setCapacity(cap_);
}


