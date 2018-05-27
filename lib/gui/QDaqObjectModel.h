#ifndef QDAQOBJECTMODEL_H
#define QDAQOBJECTMODEL_H

#include <QAbstractItemModel>

#include "QDaqGlobal.h"

class QDaqObject;

/** Class QDaqObjectModel represents the QDaq object tree as a Qt item model.
 *
 * This class enables the visual representation of the QDaq object tree in
 * Qt's model/view classes.
 *
 * The class is just a wrapper around the QDaqObject hierarchy.
 * No extra data is generated for the class.
 *
 */
class QDAQ_EXPORT QDaqObjectModel : public QAbstractItemModel
{
    Q_OBJECT

    void* rootNode;

public:
    enum { NumColumns = 1 };

    explicit QDaqObjectModel(QObject *parent = 0);
    virtual ~QDaqObjectModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(const QDaqObject*, int column = 0) const;
    QModelIndex index(const QString &path, int column = 0) const;
    QDaqObject * objectAt(const QModelIndex &index) const;

private slots:
    void insert(QDaqObject* obj);
    void remove(QDaqObject* obj);
};

#endif // QDAQOBJECTMODEL_H
