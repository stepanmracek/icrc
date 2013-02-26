#ifndef ICA_H
#define ICA_H

#include <opencv/cv.h>
#include <vector>

#include "common.h"
#include "backprojectionbase.h"

/**
 * @brief Implementation of independent component analysis (ICA)
 */
class ICA : public BackProjectionBase
{
    Q_OBJECT

private:
    MatF EDET;
    MatF EDETinv;
    MatF W;
    MatF mean;
    MatF variances;

public:
    /**
     * @brief Basic constructor
     * @param parent parent object in Qt hierarchy
     */
    ICA(QObject *parent = 0) : BackProjectionBase(parent) { }

    /**
     * @brief Constructor that calculates ICA of input vectors
     * @param vectors input vectors
     * @param independentComponentCount maximum count of independent components, set to 0 if you want all components
     * @param eps update threshold where learning stops
     * @param maxIterations maximum number of learning iterations
     * @param debug set to true if you want to see debug output
     * @param parent parent object in Qt hierarchy
     */
    ICA(std::vector<MatF> &vectors, int independentComponentCount = 0, double eps = 1e-10,
        int maxIterations = 10000, bool debug = false, QObject *parent = 0);

    /**
     * @brief ICA Constructor that loads the ICA from the given file
     * @param path File with serialized ICA data
     * @param parent parent object in Qt hierarchy
     */
    ICA(const QString &path, QObject *parent = 0);

    int getModes() { return W.rows; }

    void setModes(int /*modes*/) { /* TODO!!! */ }

    /**
     * @brief learn the ICA from given data
     * @param vectors input vectors/samples/data
     * @param independentComponentCount maximum count of independent components, set to 0 if you want all components
     * @param eps update threshold where learning stops
     * @param maxIterations maximum number of learning iterations
     * @param debug set to true if you want to see debug output
     */
    void learn(const std::vector<MatF> &vectors, int independentComponentCount = 0, double eps = 1e-10, int maxIterations = 10000, bool debug = false);

    void learn(const std::vector<MatF> &vectors) { learn(vectors, 0, 1e-10, 10000, false); }

    void serialize(const QString &path);

    void deserialize(const QString &path);

    MatF project(const MatF &vector);

    MatF backProject(const MatF &vector);

    float getVariance(int mode) { return variances(mode); }

    MatF getMean() { return mean; }

private:
    MatF whiten(const MatF &vector);
    void calculateVariances(const std::vector<MatF> &vectors);
};

#endif // ICA_H
