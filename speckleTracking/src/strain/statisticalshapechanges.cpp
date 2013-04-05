#include "statisticalshapechanges.h"

#include <linalg/pca.h>

#include <cassert>

std::vector<Points> StatisticalShapeChanges::createDeltas(VectorOfShapes &shapes)
{
    std::vector<Points> result;

    int m = shapes.front().size();
    float prevX[m];
    float prevY[m];
    bool first = true;
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &curPoints = (*it);
        if (!first)
        {
            Points newDeltas;
            for (int i = 0; i < m; i++)
            {
                P delta;
                delta.x = curPoints[i].x - prevX[i];
                delta.y = curPoints[i].y - prevY[i];
                newDeltas.push_back(delta);
            }

            result.push_back(newDeltas);
        }
        first = false;

        for (int i = 0; i < m; i++)
        {
            prevX[i] = curPoints[i].x;
            prevY[i] = curPoints[i].y;
        }
    }

    return result;
}

StatisticalShapeChanges::StatisticalShapeChanges(VectorOfShapes &shapes, int framesCount, BackProjectionBase &model) :
    model(model), framesCount(framesCount)
{
    int n = shapes.size();
    assert(n > 2);
    assert(framesCount >= 2);
    assert(framesCount < n);

    std::vector<Points> deltas = createDeltas(shapes);
    int deltasCount = deltas.size();

    int pointCount = deltas[0].size();

    std::vector<MatF> deltaVectors;

    //std::cout << "shapes count: " << n << std::endl;
    //std::cout << "deltas count: " << deltasCount << std::endl;
    for (int i = 0; i <= (deltasCount - framesCount); i++)
    {
        MatF vector = MatF::zeros(pointCount*framesCount*2, 1);
        int row = 0;
        for (int j = i; j < i + framesCount; j++)
        {
            // append all "j" frames to one column vector
            //std::cout << j << " ";

            Points & delta = deltas[j];
            for (int k = 0; k < pointCount; k++)
            {
                vector(row) = delta[k].x;
                row++;
                vector(row) = delta[k].y;
                row++;
            }
        }
        deltaVectors.push_back(vector);
        //std::cout << std::endl;
    }

    model.learn(deltaVectors);
}

Points StatisticalShapeChanges::predict(std::vector<Points> &shapes)
{
    int n = shapes.size();
    // assert that just the last frame that we are predicting is missing
    assert (framesCount == n + 1);
    int pointCount = shapes[0].size();

    MatF historyShapes = Common::pointsToMatF(shapes);
    MatF currentShape = MatF::zeros(pointCount*2, 1);
    MatF allShapes  = MatF::zeros(pointCount*2*(n+1), 1);
    historyShapes.copyTo(allShapes.rowRange(0, historyShapes.rows));

    for (int i = 0; i < 1000; i++)
    {
        MatF projected = model.project(allShapes);
        model.threeSigmaNormalization(projected);
        MatF backProjected = model.backProject(projected);

        assert(backProjected.rows == allShapes.rows);

        MatF newCurrentShape = backProjected.rowRange(historyShapes.rows, backProjected.rows);
        MatF diffShape = newCurrentShape - currentShape;
        float absDiff = Common::absSum(diffShape);
        //std::cout << "   iteration " << i << "; diff " << absDiff << std::endl;

        newCurrentShape.copyTo(currentShape);
        historyShapes.copyTo(allShapes.rowRange(0, historyShapes.rows));
        currentShape.copyTo(allShapes.rowRange(historyShapes.rows, allShapes.rows));

        if (absDiff < 1e-10)
        {
            break;
        }
    }

    std::vector<Points> resultVec = Common::matFToPoints(currentShape, 1);
    Points result = resultVec[0];
    return result;
}
