#ifndef ICA_H
#define ICA_H

#include <opencv/cv.h>
#include <vector>

#include "common.h"
#include "backprojectionbase.h"

class ICA : public BackProjectionBase
{
private:
    MatF EDET;
    MatF EDETinv;
    MatF W;
    MatF mean;
    MatF variances;

public:
    ICA() {}

    ICA(std::vector<MatF> &vectors, int independentComponentCount = 0, double eps = 1e-10, int maxIterations = 10000, bool debug = false);

    ICA(const char *path);

    int getModes() { return W.rows; }

    void setModes(int /*modes*/) { /* TODO!!! */ }

    void learn(const std::vector<MatF> &vectors, int independentComponentCount = 0, double eps = 1e-10, int maxIterations = 10000, bool debug = false);

    void learn(const std::vector<MatF> &vectors) { learn(vectors, 0, 1e-10, 10000, false); }

    void calculateVariances(const std::vector<MatF> &vectors);

    void serialize(const char *path);

    void deserialize(const char *path);

    MatF whiten(const MatF &vector);

    MatF project(const MatF &vector);

    MatF backProject(const MatF &vector);

    float getVariance(int mode) { return variances(mode); }

    MatF getMean() { return mean; }
};

#endif // ICA_H
