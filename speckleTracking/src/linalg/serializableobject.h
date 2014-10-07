#ifndef SERIALIZABLEOBJECT_H
#define SERIALIZABLEOBJECT_H

#include <QObject>
#include <QString>
#include <opencv2/core/core.hpp>

/**
 * @brief Class represents serializable and deserializable object
 */
class SerializableObject : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief SerializableObject constructor
     * @param parent Parent object in Qt hieararchy
     */
    SerializableObject(QObject *parent = 0) : QObject(parent) { }

    /**
     * @brief Serializes internal data of object to the given file
     * @param path Desired file path
     */
    virtual void serialize(cv::FileStorage &storage) const = 0;

    /**
     * @brief Deserializes internal data from file to object
     * @param path Desired file path
     */
    virtual void deserialize(cv::FileStorage &storage) = 0;
};

#endif // SERIALIZABLEOBJECT_H
