#include "shapecomparer.h"

#include "procrustes.h";

float ShapeComparer::directDiff(const Points &first, const Points &second)
{
    int n = first.size();
    assert(second.size() == n);

    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        const P &p1 = first[i];
        const P &p2 = second[i];
        sum += (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
    }

    sum /= n;
    return sum;
}

float ShapeComparer::optimalDiff(const Points &first, const Points &second)
{
    MatF firstMat = Common::pointsToMatF(first);
    MatF secondMat = Common::pointsToMatF(second);
    Procrustes::centralize(firstMat);
    Procrustes::centralize(secondMat);

    float theta = Procrustes::getOptimalRotation(firstMat, secondMat);
    Procrustes::rotateAndScale(firstMat, ScaleAndRotateCoefs(1, theta));

    return directDiff(Common::matFToPoints(firstMat), Common::matFToPoints(secondMat));
}
