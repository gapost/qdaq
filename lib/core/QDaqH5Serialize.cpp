#include "QDaqObject.h"
#include "QDaqRoot.h"
#include "QDaqDataBuffer.h"
#include "qdaqh5file.h"

#include <QDebug>



#define CLASS_ATTR_NAME "Class"

void QDaqObject::writeh5(Group *h5g, QDaqH5File *f) const
{
    f->helper()->writeProperties(h5g,this,metaObject());
}

void QDaqObject::readh5(Group *g, QDaqH5File *f)
{
    f->helper()->readProperties(g,this);
}

void QDaqDataBuffer::writeh5(H5::Group* h5g, QDaqH5File *f) const
{

    f->helper()->lockedPropertyList(columnNames_);

    QDaqObject::writeh5(h5g,f);

    f->helper()->lockedPropertyList();


    if (!(columns() && size())) return;

    hsize_t dims = size();
    DataSpace space(1,&dims);
    for(uint j=0; j<columns(); j++)
    {
        QString col_name = columnNames().at(j);
        DataSet ds = h5g->createDataSet(col_name.toLatin1().constData(),
                                        PredType::NATIVE_DOUBLE, space);

        ds.write(data_matrix[j].constData(),PredType::NATIVE_DOUBLE,space);
    }
}

void QDaqDataBuffer::readh5(H5::Group *g, QDaqH5File *f)
{
    QDaqObject::readh5(g,f);

    QStringList S;
    if ( f->helper()->read(g,"columnNames",S) ) columnNames_ = S;

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
        QString col_name = columnNames().at(j);
        DataSet ds = g->openDataSet(col_name.toLatin1().constData());
        DataSpace space = ds.getSpace();
        hsize_t sz;
        space.getSimpleExtentDims(&sz);

        rbuff.setCapacity((int)sz);
        ds.read(rbuff.data(),PredType::NATIVE_DOUBLE,space);
        data_matrix[j] = rbuff;
    }
    for(int i=0; i<data_matrix.size(); i++)
        data_matrix[i].setCapacity(cap_);
}


