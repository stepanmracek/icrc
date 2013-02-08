#ifndef BACKPROJECTIONBASE_H
#define BACKPROJECTIONBASE_H

#include <vector>

#include "common.h"
#include "projectionbase.h"

class BackProjectionBase : public ProjectionBase
{
public:
    virtual MatF backProject(const MatF &vector) = 0;

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
