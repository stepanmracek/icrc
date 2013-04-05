#ifndef MODELLISTOFINTS_H
#define MODELLISTOFINTS_H

#include <QVector>
#include <QAbstractItemModel>

class ModelListOfInts : public QAbstractListModel
{
private:
    QVector<int> &list;

public:
    ModelListOfInts(QVector<int> &list) : list(list) { }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if(role != Qt::DisplayRole && role != Qt::EditRole)
            return QVariant();

        if(index.column() == 0 && index.row() < list.count())
            return list.at(index.row());
        else
            return QVariant();
    }

    int rowCount(const QModelIndex &parent) const
    {
        return list.count();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    {
        if(role == Qt::EditRole)
        if(role != Qt::EditRole || index.column() != 0 || index.row() >= list.count())
            return false;

        if(value.toInt() == list.at(index.row()))
            return false;

        list[index.row()] = value.toInt();

        emit dataChanged(index, index);
        return true;
    }

    bool insertRows(int position, int count, const QModelIndex &parent = QModelIndex())
    {
        beginInsertRows(QModelIndex(), position, position+count-1);

        for (int row = 0; row < count; ++row) {
            list.insert(position, 0);
        }
        endInsertRows();

        return true;
    }

    bool removeRows(int position, int count, const QModelIndex &parent)
    {
        beginRemoveRows(QModelIndex(), position, position+count-1);

        for (int row = 0; row < count; ++row) {
            list.remove(position);
        }

        endRemoveRows();
        return true;
    }

    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const
    {
        return createIndex(row, column);
    }
};

#endif // MODELLISTOFINTS_H