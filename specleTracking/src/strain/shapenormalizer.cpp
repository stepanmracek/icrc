#include "shapenormalizer.h"

#include <cfloat>

Points ShapeNormalizerIterConfStatShape::normalize(Points &points, Mat8 &frame)
{
    unsigned int n = points.size();
    assert(n > 0);

    VectorF confidence(n);
    float eps = n * 0.001;
    float sumOfDistances = eps;

    // we work on copy
    Points result = points;

    int counter = 0;
    while (sumOfDistances >= eps)
    {
        // debug
        counter++;
        //std::cout << "ShapeNormalizerIterConfStatShape::normalize " << counter << " " << sumOfDistances << std::endl;

        // ******
        //  TODO! Inspect whether it is better to have stable confidence for every iteration
        // ******
        // Calculate confidence for each point that is based on the intensity;
        for (unsigned int i = 0; i < n; i++)
        {
            P& p  = result[i];
            if (p.x < 0 || p.y < 0 || p.x >= frame.cols || p.y >= frame.rows)
                confidence[i] = 0;
            else
                confidence[i] = frame(p)/255.0f;
        }

        // normalize
        //std::cout << "  preNormalize" << std::endl;
        Points newPositions = model->iterativeNormalize(result);
        //std::cout << "  postNormalize" << std::endl;

        // for each point
        sumOfDistances = 0;
        assert(n == result.size());
        assert(n == confidence.size());
        assert(n == newPositions.size());
        for (int i = 0; i < n; i++)
        {
            // move according to the confidence
            P newPos = P(result[i].x + (1-confidence[i])*(newPositions[i].x - result[i].x),
                         result[i].y + (1-confidence[i])*(newPositions[i].y - result[i].y));

            // calculate distance between new point and old point
            float d = Common::eucl(newPos, result[i]);
            sumOfDistances += d;

            // assign
            result[i] = newPos;
        }
        //std::cout << "  iteration ended " << sumOfDistances << std::endl;
    }

    return result;
}
