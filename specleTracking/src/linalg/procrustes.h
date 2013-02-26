#ifndef PROCRUSTES_H
#define PROCRUSTES_H

#include <opencv2/opencv.hpp>
#include <climits>
#include <vector>

#include "common.h"

/**
 * @brief The ScaleAndRotateCoefs struct
 */
struct ScaleAndRotateCoefs
{
    /**
     * @brief ScaleAndRotateCoefs default constructor. Initializes scale to 1 and rotation to 0
     */
    ScaleAndRotateCoefs()
    {
        s = 1;
        theta = 0;
    }

    /**
     * @brief ScaleAndRotateCoefs constructor
     * @param s Scale
     * @param theta Rotation
     */
    ScaleAndRotateCoefs(float s, float theta) : s(s), theta(theta) {}

    /**
     * @brief Calculates inverse transform
     * @return Inverse transform
     */
    ScaleAndRotateCoefs inv()
    {
        return ScaleAndRotateCoefs(1/s, -theta);
    }

    /**
     * @brief Scale
     */
    float s;

    /**
     * @brief Rotation
     */
    float theta;
};

/**
 * @brief The TranslationCoefs struct
 */
struct TranslationCoefs
{
    /**
     * @brief TranslationCoefs default constructor. Initializes to zero translation
     */
    TranslationCoefs()
    {
        xt = 0.0;
        yt = 0.0;
    }

    /**
     * @brief TranslationCoefs constructor
     * @param xt x-axis shift
     * @param yt y-axis shift
     */
    TranslationCoefs(float xt, float yt) : xt(xt), yt(yt) {}

    /**
     * @brief Calculates inverse transform
     * @return Inverse transform
     */
    TranslationCoefs inv()
    {
        return TranslationCoefs(-xt, -yt);
    }

    /**
     * @brief x-axis shift
     */
    float xt;

    /**
     * @brief y-axis shift
     */
    float yt;
};

/**
 * @brief Struct represents general linear 2D transformation (scale,rotation,shift)
 */
struct TransformationCoefs
{
    /**
     * @brief Default constructor initializes to empty transform
     */
    TransformationCoefs()
    {
        a = 1;
        b = 0;
    }

    /**
     * @brief Constructor
     * @param a
     * @param b
     */
    TransformationCoefs(float a, float b) : a(a), b(b) {}

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
