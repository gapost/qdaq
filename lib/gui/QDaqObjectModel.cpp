#include "QDaqObjectModel.h"

#include "QDaqRoot.h"

#include <QImage>

QDaqObjectModel::QDaqObjectModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    index(QDaqObject::root());
    connect(QDaqObject::root(), SIGNAL(objectCreated(QDaqObject*)),
        this, SLOT(insert(QDaqObject*)));
    connect(QDaqObject::root(), SIGNAL(objectDeleted(QDaqObject*)),
        this, SLOT(remove(QDaqObject*)));
}

QVariant QDaqObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        if (section == 0) {
            // ### TODO oh man this is ugly and doesn't even work all the way!
            // it is still 2 pixels off
            QImage pixmap(16, 1, QImage::Format_Mono);
            pixmap.fill(0);
            pixmap.setAlphaChannel(pixmap.createAlphaMask());
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    QString returnValue;
    switch (section) {
    case 0: returnValue = tr("Name");
            break;
    case 1: returnValue = tr("Class");
            break;
    default: return QVariant();
    }

    return returnValue;
}

QModelIndex QDaqObjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    // get the parent node
    QDaqObject* parentNode = (parent.isValid() ? objectAt(parent) :
                                                   const_cast<QDaqObject*>((QDaqObject*)QDaqObject::root()));
    Q_ASSERT(parentNode);

    // now get the internal pointer for the index
    const QDaqObject* indexNode = parentNode->children().at(row);
    Q_ASSERT(indexNode);

    return createIndex(row, column, const_cast<QDaqObject*>(indexNode));
}

QModelIndex QDaqObjectModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

    QDaqObject* indexNode = objectAt(index);

    Q_ASSERT(indexNode != 0);

    QDaqObject* parentNode = indexNode->parent();
    if (parentNode == 0 || parentNode == QDaqObject::root())
        return QModelIndex();

    // get the parent's row
    QDaqObject* grandParentNode = parentNode->parent();

    Q_ASSERT(grandParentNode->children().contains(parentNode));

    int row = grandParentNode->children().indexOf(parentNode);

    if (row == -1)
        return QModelIndex();

    return createIndex(row, 0, parentNode);
}

int QDaqObjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
         return 0;

    QDaqObject* obj;
    if (!parent.isValid()) obj = QDaqObject::root();
    else obj = objectAt(parent);

    int count = obj->children().size();
    return count;
}

int QDaqObjectModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : NumColumns;
}

bool QDaqObjectModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return false;

    const QDaqObject *indexNode = objectAt(parent);
    Q_ASSERT(indexNode);
    return (!indexNode->children().isEmpty());
}

bool QDaqObjectModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    Q_UNUSED(parent)
    return false;
}

void QDaqObjectModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
    Q_UNUSED(parent)
}

QVariant QDaqObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return objectAt(index)->objectName();
        case 1: return objectAt(index)->metaObject()->className();
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
        }
        break;
    case Qt::DecorationRole:
//        if (index.column() == 0) {
//            QIcon icon; // = d->icon(index);
//            return icon;
//        }
        break;
//    case Qt::TextAlignmentRole:
//        if (index.column() == 1)
//            return Qt::AlignRight;
//        break;
    }

    return QVariant();
}

void QDaqObjectModel::insert(QDaqObject *obj)
{
    QDaqObject* parent = obj->parent();
    int row = parent->children().indexOf(obj);
    Q_ASSERT(parent);
    QModelIndex parentIndex = index(parent);
    beginInsertRows(parentIndex, row, row);
    endInsertRows();
}
void QDaqObjectModel::remove(QDaqObject *obj)
{
    QDaqObject* parent = obj->parent();
    int row = parent->children().indexOf(obj);
    Q_ASSERT(parent);
    QModelIndex parentIndex = index(parent);
    beginRemoveRows(parentIndex, row, row);
    endRemoveRows();
}

QModelIndex QDaqObjectModel::index(const QDaqObject* obj, int column) const
{
    QDaqObject *parent = (obj ? obj->parent() : 0);
    if (obj == QDaqObject::root() || !parent)
        return QModelIndex();

    // get the parent's row
    Q_ASSERT(obj);
    int row = 0;
    QDaqObjectList children = parent->children();
    foreach(const QDaqObject* ch, children)
    {
        if (ch==obj) break;
        row++;
    }
    Q_ASSERT(row<children.size());
    return createIndex(row, column, const_cast<QDaqObject*>(obj));
}

QModelIndex QDaqObjectModel::index(const QString &path, int column) const
{
    QDaqObject* obj = QDaqObject::findByName(path);
    return index(obj,column);
}

QDaqObject* QDaqObjectModel::objectAt(const QModelIndex &index) const
{
    if (!index.isValid())
        return QDaqObject::root(); //const_cast<QFileSystemNode*>(&root);
    QDaqObject* obj = static_cast<QDaqObject*>(index.internalPointer());
    Q_ASSERT(obj);
    return obj;
}

