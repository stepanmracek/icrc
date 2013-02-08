#ifndef PROCRUSTES_H
#define PROCRUSTES_H

#include <opencv2/opencv.hpp>
#include <climits>
#include <vector>

#include "common.h"

struct ScaleAndRotateCoefs
{
    ScaleAndRotateCoefs()
    {
        s = 1;
        theta = 0;
    }

    ScaleAndRotateCoefs(float _s, float _theta) : s(_s), theta(_theta) {}

    ScaleAndRotateCoefs inv()
    {
        return ScaleAndRotateCoefs(1/s, -theta);
    }

    float s;
    float theta;
};

struct TranslationCoefs
{
    TranslationCoefs()
    {
        xt = 0.0;
        yt = 0.0;
    }

    TranslationCoefs(float _xt, float _yt) : xt(_xt), yt(_yt) {}

    TranslationCoefs inv()
    {
        return TranslationCoefs(-xt, -yt);
    }

    float xt;
    float yt;
};

struct TransformationCoefs
{
    TransformationCoefs()
    {
        a = 1;
        b = 0;
    }

    TransformationCoefs(float _a, float _b) : a(_a), b(_b) {}

    float a;
    float b;
};

class Procrustes
{
public:
    static void procrustesAnalysis(std::vector<MatF> &vectors, bool scale = true,
    							   float eps = 0, int maxIterations = INT_MAX);

    static float getOptimalRotation(MatF &from, MatF &to);

    static ScaleAndRotateCoefs align(MatF &from, MatF &to);

    static void rotateAndScale(MatF &vector, ScaleAndRotateCoefs &coefs);

    static void transformate(MatF &vector, TransformationCoefs &coefs);

    static void translate(MatF &vector, TranslationCoefs &coefs);

    static TranslationCoefs centralizedTranslation(MatF &vector);

    static void centralize(MatF &vector);

    static void centralize(std::vector<MatF> &vectors);

    static MatF getMeanShape(std::vector<MatF> &vectors);
};

#endif // PROCRUSTES_H
