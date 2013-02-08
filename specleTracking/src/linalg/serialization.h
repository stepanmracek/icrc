#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <QMap>
#include <QVector>
#include <vector>
#include <list>

#include "common.h"

/**
 * @brief The Serialization class for storing and reading data
 */
class Serialization
{
public:
    // Serialization methods

    /**
     * @brief Serialize the matrix to the given file
     * @param m Input matrix
     * @param path Desired path of the file
     * @return Returns true if the serialization was successfull
     */
    static bool serialize(MatF &m, const char *path);

    /**
     * @brief Serialize the shape to the given file
     * @param shape Input shape
     * @param path Desired path of the file
     * @return Returns true if the serialization was successfull
     */
    static bool serialize(Points &shape, const char *path);

    /**
     * @brief Serialize the list of shapes to the given file
     * @param shapes Input list of shapes
     * @param path Desired path of the file
     * @return Returns true if the serialization was successfull
     */
    static bool serialize(VectorOfShapes &shapes, const char *path);

    /**
     * @brief Serialize the map of shapes to the given file
     * @param shapes Input maps of shapes
     * @param path Desired path of the file
     * @return Returns true if the serialization was successfull
     */
    static bool serialize(QMap<int, Points> &shapes, const char *path);

    // Deserialization methods

    /**
     * @brief Read the matrix from the file
     * @param path Input file where the matrix is stored
     * @return Returns loaded matrix
     */
    static MatF readMatF(const char *path);

    /**
     * @brief Read the shape from the file
     * @param path Input file where the shape is stored
     * @return Returns loaded shape
     */
    static Points readShape(const char *path);

    /**
     * @brief Read the list of shapes from the file
     * @param path Input file where the shapes are stored
     * @return Returns list of loaded shapes
     */
    static VectorOfShapes readShapeList(const char *path);

    /**
     * @brief Read the map of shapes from the file
     * @param path Input file where the shapes are stored
     * @return Returns map of loaded shapes
     */
    static QMap<int, Points> readShapeMap(const char *path);
};

#endif // SERIALIZATION_H
