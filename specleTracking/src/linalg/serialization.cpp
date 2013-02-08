#include "serialization.h"

bool Serialization::serialize(MatF &m, const char *path)
{
    cv::FileStorage storage(path, cv::FileStorage::WRITE);
    if (!storage.isOpened()) return false;
    storage << "m" << m;
    return true;
}

bool Serialization::serialize(Points &shape, const char *path)
{
    MatF m = Common::pointsToMatF(shape);
    return serialize(m, path);
}

bool Serialization::serialize(VectorOfShapes &shapes, const char *path)
{
    cv::FileStorage storage(path, cv::FileStorage::WRITE);
    if (!storage.isOpened()) return false;
    storage << "shapes" << "[";
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        MatF m = Common::pointsToMatF(*it);
        storage << m;
    }
    storage << "]";
    return true;
}

bool Serialization::serialize(QMap<int, Points> &shapes, const char *path)
{
    cv::FileStorage storage(path, cv::FileStorage::WRITE);
    if (!storage.isOpened()) return false;

    QList<int> keys = shapes.keys();
    storage << "keys" << "[";
    foreach(int k, keys)
    {
        storage << k;
    }
    storage << "]";

    storage << "shapes" << "[";
    foreach(int k, keys)
    {
        MatF m = Common::pointsToMatF(shapes[k]);
        storage << m;
    }
    storage << "]";

    return true;
}

MatF Serialization::readMatF(const char *path)
{
    cv::FileStorage storage(path, cv::FileStorage::READ);
    MatF m;
    storage["m"] >> m;
    return m;
}

Points Serialization::readShape(const char *path)
{
    MatF m = readMatF(path);
    return Common::matFToPoints(m);
}

VectorOfShapes Serialization::readShapeList(const char *path)
{
    VectorOfShapes result;
    cv::FileStorage storage(path, cv::FileStorage::READ);
    cv::FileNode node = storage["shapes"];
    for (cv::FileNodeIterator it = node.begin(); it != node.end(); ++it)
    {
        MatF m;
        (*it) >> m;
        Points shape = Common::matFToPoints(m);
        result.push_back(shape);
    }

    return result;
}

QMap<int, Points> Serialization::readShapeMap(const char *path)
{
    QMap<int, Points> result;
    cv::FileStorage storage(path, cv::FileStorage::READ);

    QVector<int> keys;
    cv::FileNode keysNode = storage["keys"];
    for (cv::FileNodeIterator it = keysNode.begin(); it != keysNode.end(); ++it)
    {
        int key;
        (*it) >> key;
        keys << key;
    }

    cv::FileNode shapesNode = storage["shapes"];
    int index = 0;
    for (cv::FileNodeIterator it = shapesNode.begin(); it != shapesNode.end(); ++it)
    {
        MatF m;
        (*it) >> m;
        Points shape = Common::matFToPoints(m);

        result[keys[index]] = shape;
        index++;
    }

    return result;
}
