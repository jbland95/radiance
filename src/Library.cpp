#include "Library.h"
#include "Paths.h"
#include "Registry.h"
#include <QDir>
#include <QDebug>

LibraryItem::LibraryItem(const QString &name, const QString &fileToInstantiate, LibraryItem *parent)
{
    m_parentItem = parent;
    m_name = name;
    m_file = fileToInstantiate;
}

LibraryItem::~LibraryItem()
{
    qDeleteAll(m_childItems);
}

void LibraryItem::appendChild(LibraryItem *item)
{
    m_childItems.append(item);
}

LibraryItem *LibraryItem::child(int row)
{
    return m_childItems.value(row);
}

int LibraryItem::childCount() const
{
    return m_childItems.count();
}

int LibraryItem::row() const
{
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<LibraryItem*>(this));
    }

    return 0;
}

QString LibraryItem::name() const
{
    return m_name;
}

QString LibraryItem::file() const
{
    return m_file;
}

LibraryItem *LibraryItem::parentItem()
{
    return m_parentItem;
}

Library::Library(Registry *registry)
    : QAbstractItemModel()
    , m_rootItem(nullptr)
    , m_registry(registry) {

    rebuild();
}

Library::~Library()
{
    delete m_rootItem;
}

LibraryItem *Library::itemFromFile(QString filename, LibraryItem *parent) {
    if (!m_registry->canCreateFromFile(filename)) return nullptr;
    return new LibraryItem(QFileInfo(filename).baseName(), filename, parent);
}

void Library::populate(LibraryItem *item, QString currentDirectory) {
    QDir d(currentDirectory);
    qDebug() << d;
    auto ls = d.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    qDebug() << ls;
    for (auto f = ls.begin(); f != ls.end(); f++) {
        auto fullPath = currentDirectory + "/" + *f;
        if (QDir(fullPath).exists()) {
            auto newItem = new LibraryItem(*f, "", item);
            item->appendChild(newItem);
            populate(newItem, fullPath);
        } else {
            auto newItem = itemFromFile(fullPath, item);
            if (newItem != nullptr) {
                item->appendChild(newItem);
            }
        }
    }
}

void Library::rebuild() {
    delete m_rootItem;
    m_rootItem = new LibraryItem("", "");
    populate(m_rootItem, Paths::library());
}

QVariant Library::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    LibraryItem *item = static_cast<LibraryItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        return item->name();
    } else if (role == Library::FileRole) {
        return item->file();
    } else {
        return QVariant();
    }

}

QModelIndex Library::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    LibraryItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<LibraryItem*>(parent.internalPointer());

    LibraryItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex Library::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    LibraryItem *childItem = static_cast<LibraryItem*>(index.internalPointer());
    LibraryItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int Library::rowCount(const QModelIndex &parent) const {
    LibraryItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<LibraryItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int Library::columnCount(const QModelIndex &parent) const {
    return 1;
}

Qt::ItemFlags Library::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QHash<int, QByteArray> Library::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "name";
    roles[FileRole] = "file";
    return roles;
}