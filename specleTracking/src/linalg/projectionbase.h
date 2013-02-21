#ifndef PROJECTIONBASE_H
#define PROJECTIONBASE_H

#include <vector>

#include "common.h"
#include "serializableobject.h"

class ProjectionBase : public SerializableObject
{
    Q_OBJECT

public:
    ProjectionBase(QObject *parent = 0) : SerializableObject(parent) { }

    virtual MatF project(const MatF &vector) = 0;

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

    virtual void learn(const std::vector<MatF> &vectors) = 0;
    virtual float getVariance(int mode) = 0;
    virtual MatF getMean() = 0;
    virtual int getModes() = 0;

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
