#include "QDaqObjectModel.h"

#include "QDaqRoot.h"

#include <QImage>

QDaqObjectModel::QDaqObjectModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    //index(QDaqObject::root());
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

    if (!parent.isValid())
        return createIndex(0, column, QDaqObject::root());

    // get the parent node
    QDaqObject* parentNode = objectAt(parent);
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

    if (indexNode == QDaqObject::root())
        return QModelIndex();

    QDaqObject* parentNode = indexNode->parent();
    if (parentNode == QDaqObject::root())
        return createIndex(0, 0, parentNode);

    // get the parent's row
    QDaqObject* grandParentNode = parentNode->parent();
    Q_ASSERT(grandParentNode);

    int row = grandParentNode->children().indexOf(parentNode);
    if (row == -1)
        return QModelIndex();

    return createIndex(row, 0, parentNode);
}

int QDaqObjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
         return 0;

    if (!parent.isValid()) return 1;

    QDaqObject* obj = objectAt(parent);
    Q_ASSERT(obj);

    return obj->children().size();
}

int QDaqObjectModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : NumColumns;
}

bool QDaqObjectModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return false;

    if (!parent.isValid()) return true;
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
    Q_ASSERT(parent);
    int row = parent->children().indexOf(obj);
    QModelIndex parentIndex = index(parent);
    beginInsertRows(parentIndex, row, row);
    endInsertRows();
}
void QDaqObjectModel::remove(QDaqObject *obj)
{
    QDaqObject* parent = obj->parent();
    Q_ASSERT(parent);
    int row = parent->children().indexOf(obj);
    QModelIndex parentIndex = index(parent);
    beginRemoveRows(parentIndex, row, row);
    endRemoveRows();
}

QModelIndex QDaqObjectModel::index(const QDaqObject* obj, int column) const
{
    if (!obj) return QModelIndex();

    if (obj == QDaqObject::root()) return createIndex(0, column, const_cast<QDaqObject*>(obj));

    QDaqObject *parent = obj->parent();

    // get the parent's row
    Q_ASSERT(parent);
    int row = parent->children().indexOf(const_cast<QDaqObject*>(obj));
    Q_ASSERT(row>=0);
    return createIndex(row, column, const_cast<QDaqObject*>(obj));
}

QModelIndex QDaqObjectModel::index(const QString &path, int column) const
{
    QDaqObject* obj = QDaqObject::findByName(path);
    return index(obj,column);
}

QDaqObject* QDaqObjectModel::objectAt(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    QDaqObject* obj = static_cast<QDaqObject*>(index.internalPointer());
    Q_ASSERT(obj);
    return obj;
}

