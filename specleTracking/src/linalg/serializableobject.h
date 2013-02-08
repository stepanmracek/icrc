#ifndef SERIALIZABLEOBJECT_H
#define SERIALIZABLEOBJECT_H

#include <QString>

class SerializableObject
{
public:
    virtual void serialize(const QString &path) = 0;
    virtual void deserialize(const QString &path) = 0;
};

#endif // SERIALIZABLEOBJECT_H
