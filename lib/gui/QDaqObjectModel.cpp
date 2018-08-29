#include "QDaqObjectModel.h"

#include "QDaqRoot.h"
#include "QDaqJob.h"
#include "QDaqChannel.h"
#include "QDaqInterface.h"
#include "QDaqDevice.h"
#include "QDaqFilter.h"

#include <QImage>
#include <QIcon>


class Node
{
public:
    QList<Node*> children;
    QDaqObject *objptr;
    Node *parent;

    explicit Node(QDaqObject *obj, Node *parentItem = 0) :
        objptr(obj),
        parent(parentItem)
    {}

    void populate()
    {
        foreach (QDaqObject* ch, objptr->children()) {
            Node* n = new Node(ch,this);
            children.push_back(n);
            n->populate();
        }
    }
    void deleteChildren()
    {
        foreach (Node* n, children) {
            n->deleteChildren();
            delete n;
        }
    }

    void appendChild(Node *child)
    {
        children.push_back(child);
    }
    void insertChild(int row, Node* child)
    {
        children.insert(row,child);
    }

    Node *child(int row)
    {
        return children.at(row);
    }
    int childCount() const
    {
        return children.size();
    }
    int row()
    {
        if (!parent) return -1;
        return parent->children.indexOf(this);
    }
    Node* nodeOfObject(const QDaqObject* obj)
    {
        if (obj==objptr) return this;
        Node* n(0);
        foreach (Node* m, children) {
            n = m->nodeOfObject(obj);
            if (n) break;
        }
        return n;
    }
};

QDaqObjectModel::QDaqObjectModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootNode = new Node(QDaqObject::root());
    ((Node*)rootNode)->populate();

    connect(QDaqObject::root(), SIGNAL(objectAttached(QDaqObject*)),
        this, SLOT(insert(QDaqObject*)));
    connect(QDaqObject::root(), SIGNAL(objectDetached(QDaqObject*)),
        this, SLOT(remove(QDaqObject*)));
}

QDaqObjectModel::~QDaqObjectModel()
{
    Node* ptr = (Node*)rootNode;
    ptr->deleteChildren();
    delete ptr;
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
        return createIndex(0, column, rootNode);

    // get the parent node
    Node* parentNode = (Node*)parent.internalPointer();
    Q_ASSERT(parentNode);

    // now get the internal pointer for the index
    const Node* indexNode = parentNode->children.at(row);
    Q_ASSERT(indexNode);

    return createIndex(row, column, const_cast<Node*>(indexNode));
}

QModelIndex QDaqObjectModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

    Node* indexNode = (Node*)index.internalPointer();
    Q_ASSERT(indexNode != 0);

    if (indexNode == rootNode)
        return QModelIndex();

    Node* parentNode = indexNode->parent;
    Q_ASSERT(parentNode);

    if (parentNode == rootNode)
        return createIndex(0, 0, parentNode);

    // get the parent's row
    int row = parentNode->row();
    if (row == -1)
        return QModelIndex();

    return createIndex(row, 0, parentNode);
}

int QDaqObjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
         return 0;

    if (!parent.isValid()) return 1;

    Node* parentNode = (Node*)parent.internalPointer();
    Q_ASSERT(parentNode);

    return parentNode->childCount();
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

    Node* parentNode = (Node*)parent.internalPointer();
    Q_ASSERT(parentNode);

    return (!parentNode->children.isEmpty());
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

QIcon objectIcon(const QDaqObject* obj)
{
    if (qobject_cast<const QDaqChannel*>(obj)) return QIcon(":/images/channel.png");
    else if (qobject_cast<const QDaqFilter*>(obj)) return QIcon(":/images/filter.png");
    else if (qobject_cast<const QDaqInterface*>(obj)) return QIcon(":/images/network.png");
    else if (qobject_cast<const QDaqDevice*>(obj)) return QIcon(":/images/device.png");
    else if (qobject_cast<const QDaqLoop*>(obj)) return QIcon(":/images/loop.png");
    else if (qobject_cast<const QDaqJob*>(obj)) return QIcon(":/images/gear.png");
    else return QIcon(":/images/cube.png");
}

QVariant QDaqObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    QDaqObject* obj = objectAt(index);
    Q_ASSERT(obj);

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return obj->objectName();
        case 1: return obj->metaObject()->className();
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0) {
            return objectIcon(obj);
        }
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
    Node* parentNode = ((Node*)rootNode)->nodeOfObject(parent);
    int nch = parentNode->childCount();
    Node* objNode = new Node(obj,parentNode);

    if (row>=nch) {
        row = nch;
        beginInsertRows(parentIndex, row, row);
        parentNode->appendChild(objNode);
    }
    else {
        beginInsertRows(parentIndex, row, row);
        parentNode->insertChild(row,objNode);
    }

    endInsertRows();
}
void QDaqObjectModel::remove(QDaqObject *obj)
{
    QDaqObject* parent = obj->parent();
    Q_ASSERT(parent);

    QModelIndex parentIndex = index(parent);
    Node* parentNode = ((Node*)rootNode)->nodeOfObject(parent);
    Node* objNode = parentNode->nodeOfObject(obj);
    int row = objNode->row();

    beginRemoveRows(parentIndex, row, row);

    parentNode->children.takeAt(row);
    delete objNode;

    endRemoveRows();
}

QModelIndex QDaqObjectModel::index(const QDaqObject* obj, int column) const
{
    if (!obj) return QModelIndex();

    if (obj == QDaqObject::root()) return createIndex(0, column, rootNode);

    Q_ASSERT(obj->isAttached());

    Node* n = ((Node*)rootNode)->nodeOfObject(obj);
    Q_ASSERT(n);

    int row = n->row();
    Q_ASSERT(row>=0);
    return createIndex(row, column, n);
}

QModelIndex QDaqObjectModel::index(const QString &path, int column) const
{
    QDaqObject* obj = QDaqObject::fromPath(path);
    return index(obj,column);
}

QDaqObject *QDaqObjectModel::objectAt(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    Node* nd = (Node*)index.internalPointer();
    Q_ASSERT(nd);
    //Q_ASSERT(obj->isAttached());
    return nd->objptr;
}

