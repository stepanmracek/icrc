#ifndef PCA_H
#define PCA_H

#include <opencv/cv.h>
#include <vector>

#include "common.h"
#include "backprojectionbase.h"

class PCA : public BackProjectionBase
{
    Q_OBJECT

public:
    cv::PCA cvPca;

    PCA(QObject *parent = 0) : BackProjectionBase(parent) { }

    PCA(std::vector<MatF> &vectors, int maxComponents = 0, QObject *parent = 0);

    PCA(const char *path, QObject *parent = 0);

    int getModes() { return cvPca.eigenvalues.rows; }

    float getEigenVal(int mode);

    void setModes(int modes);

    void modesSelectionThreshold(double t = 0.98);

    void learn(const std::vector<MatF> &vectors, int maxComponents = 0);

    void learn(const std::vector<MatF> &vectors) { learn(vectors, 0); }

    void serialize(const QString &path);

    void deserialize(const QString &path);

    MatF project(const MatF &vector);

    MatF backProject(const MatF &vector);

    float getVariance(int mode);

    MatF getMean();
};

#endif // PCA_H
