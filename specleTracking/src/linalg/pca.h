#ifndef PCA_H
#define PCA_H

#include <opencv/cv.h>
#include <vector>

#include "common.h"
#include "backprojectionbase.h"

/**
 * @brief Implementation of principal component analysis (PCA)
 */
class PCA : public BackProjectionBase
{
    Q_OBJECT

private:
    cv::PCA cvPca;

public:

    /**
     * @brief PCA constructor
     * @param parent Parent object in Qt hierarchy
     */
    PCA(QObject *parent = 0) : BackProjectionBase(parent) { }

    /**
     * @brief Calculates PCA of input training vectors
     * @param vectors Input training vectors
     * @param maxComponents Maximum count of PCA components. Set to 0 if you want all components
     * @param parent Parent object in Qt hierarchy
     */
    PCA(std::vector<MatF> &vectors, int maxComponents = 0, QObject *parent = 0);

    /**
     * @brief Reads PCA from input data
     * @param path
     * @param parent
     */
    PCA(const QString &path, QObject *parent = 0);

    int getModes() { return cvPca.eigenvalues.rows; }

    /**
     * @brief Returns eigenvalue of specific mode/component
     * @param mode Mode/Component
     * @return Eigenvalue
     */
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
