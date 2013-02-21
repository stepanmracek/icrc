#ifndef SERIALIZABLEOBJECT_H
#define SERIALIZABLEOBJECT_H

#include <QObject>
#include <QString>

class SerializableObject : public QObject
{
public:
    SerializableObject(QObject *parent = 0) : QObject(parent) { }

    virtual void serialize(const QString &path) = 0;
    virtual void deserialize(const QString &path) = 0;
};

#endif // SERIALIZABLEOBJECT_H
