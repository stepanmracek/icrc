#ifndef VECF_H
#define VECF_H

#include <vector>
#include <cmath>
#include <cassert>
#include <opencv2/opencv.hpp>

#include "common.h"

class VecF
{
public:

    //static MatF fromFile(const QString &path);

    static double sqrMagnitude(MatF &vector);

    static double magnitude(MatF &vector);

    static double dot(MatF &v1, MatF &v2);

    static MatF &normalize(MatF &vector);

    static MatF normalized(MatF &vector);

    static MatF &mul(MatF &vector, double value);

    static bool isZero(MatF &vector);

    static void toFile(VectorF &vector, const QString &path, bool append = false);

    static VectorF fromFile(const QString &path);

    /*static void toFileWithIndicies(MatF &vector, const QString &path, bool append = false);

    static void toFileTwoCols(MatF &vector,const QString &path, bool append = false);

    static MatF fromTwoColsFile(const QString &path);*/

    static double maxValue(MatF &vector);

    static int maxIndex(MatF &vector);

    static double minValue(MatF &vector);

    static int minIndex(MatF &vector);

    static double meanValue(MatF &vector);

    static double stdDeviation(MatF &vector);

    static MatF fromVector(VectorF &vec);

    static VectorF toVector(MatF &vector);

    static MatF normalizeComponents(MatF &vector, VectorF &minValues, VectorF &maxValues, bool fixedBounds = true);

    static MatF normalizeComponents(MatF &vector);

    static int maxIndex(VectorF &vector);

    static int minIndex(VectorF &vector);

    static double maxValue(VectorF &vector);

    static double minValue(VectorF &vector);

    static double meanValue(const VectorF &vector);

    static MatF meanVector(std::vector<MatF> &vectors);

    static double stdDeviation(VectorF &vector);

    static VectorF resample(const VectorF &vector, int samplesCount);
};

#endif // VECF_H
