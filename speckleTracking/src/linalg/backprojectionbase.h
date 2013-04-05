#ifndef BACKPROJECTIONBASE_H
#define BACKPROJECTIONBASE_H

#include <vector>

#include "common.h"
#include "projectionbase.h"

/**
 * @brief The BackProjectionBase abstract class provides common interface for probabilistic models that are able
 * to reconstruct original input vector from low-dimensional projection, i.e. ICA and PCA
 */
class BackProjectionBase : public ProjectionBase
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    BackProjectionBase(QObject *parent = 0) : ProjectionBase(parent) { }

    /**
     * @brief Projects the low-dimensional vector back to the original input space
     * @param vector low-dimensional vector
     * @return Returns back-projected vector in the original space
     */
    virtual MatF backProject(const MatF &vector) = 0;

    /**
     * @brief Batch back-projection of input low-dimensional vectors
     * @param vectors low-dimensional vectors
     * @return back-projected vectors
     */
    virtual std::vector<MatF> backProject(const std::vector<MatF> &vectors)
    {
        std::vector<MatF> result;
        int n = vectors.size();
        for (int i = 0; i < n; i++)
        {
            MatF out = project(vectors[i]);
            result.push_back(out);
        }
        return result;
    }
};

#endif // BACKPROJECTIONBASE_H
