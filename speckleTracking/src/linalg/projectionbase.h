#ifndef PROJECTIONBASE_H
#define PROJECTIONBASE_H

#include <vector>

#include "common.h"
#include "serializableobject.h"

/**
 * @brief The ProjectionBase abstract class provides common interface for all probabilistic projection models
 * such is PCA, LDA, ICA,...
 */
class ProjectionBase : public SerializableObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent parent object in Qt hierarchy
     */
    ProjectionBase(QObject *parent = 0) : SerializableObject(parent) { }

    /**
     * @brief Projects the input vector the the target low-dimensional space
     * @param vector input high-dimensional vector
     * @return resulting low-dimensional vector
     */
    virtual MatF project(const MatF &vector) = 0;

    /**
     * @brief Batch projection of high-dimension vectors to low-dimension space
     * @param vectors input vectors
     * @return resulting low-dimensional vectors
     */
    virtual std::vector<MatF> project(const std::vector<MatF> &vectors)
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

    /**
     * @brief Learns the probabilistic model from  input samples
     * @param vectors input samples
     */
    virtual void learn(const std::vector<MatF> &vectors) = 0;

    /**
     * @brief Variance of the mode/component
     * @param mode mode
     * @return variance
     */
    virtual float getVariance(int mode) = 0;

    /**
     * @brief Mean from the learned data
     * @return mean from the learned data
     */
    virtual MatF getMean() = 0;

    /**
     * @brief Count of the modes/components
     * @return Count of the modes/components
     */
    virtual int getModes() = 0;

    /**
     * @brief Normalizes the range of parametres of low-dimensional projected vector based on the
     * rule of 3 sigmas
     * @param params Input low-dimensional vector
     */
    void threeSigmaNormalization(MatF &params)
    {
        int n = params.rows;
        for (int i = 0; i < n; i++)
        {
            float p = params(i);
            float limit = 3*sqrt(getVariance(i));
            if (p > limit)
                p = limit;
            else if (p < -limit)
                p = -limit;
            params(i) = p;
        }
    }
};

#endif // PROJECTIONBASE_H
