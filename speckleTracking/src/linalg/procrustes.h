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
 * @brief Struct represents general linear 2D transformation (scale,rotation,shear)
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

/**
 * @brief The Procrustes class provides methods for 2D transofrmation and calculation of procrustes analysis
 */
class Procrustes
{
public:

    /**
     * @brief Procrustes analysis of input 2D shapes represented as column vectors
     * @param vectors Input 2D shapes represented as column vectors
     * @param scale Apply scaling?
     * @param eps Desired threshold
     * @param maxIterations Maximum number of iterations
     */
    static void procrustesAnalysis(std::vector<MatF> &vectors, bool scale = true,
    							   float eps = 0, int maxIterations = INT_MAX);

    /**
     * @brief Calculates optimal rotation from one shape to another
     * @param from The shape which is rotated towards the second shape
     * @param to Desired shape (template)
     * @return Optimal rotation in radians
     */
    static float getOptimalRotation(const MatF &from, const MatF &to);

    /**
     * @brief Calculates optimal scale and rotation from one shape to another
     * @param from The shape which is transformated towards the second shape
     * @param to Desired shape (template)
     * @return Optimal scale and rotation
     */
    static ScaleAndRotateCoefs align(const MatF &from, const MatF &to);

    /**
     * @brief Rotate and scale given shape
     * @param vector Shape represented as column vector
     * @param coefs Desired scale and rotation
     */
    static void rotateAndScale(MatF &vector, const ScaleAndRotateCoefs &coefs);

    /**
     * @brief Transformate given shape
     * @param vector Shape represented as column vector
     * @param coefs desired transformation
     */
    static void transformate(MatF &vector, const TransformationCoefs &coefs);

    /**
     * @brief Translate (move) given shape
     * @param vector Shape represented as column vector
     * @param coefs desired shift
     */
    static void translate(MatF &vector, const TranslationCoefs &coefs);


    /**
     * @brief Calculates translation that moves the shape such its centroid will be in the center of coordinates
     * @param vector Shape represented as column vector
     * @return Calculated translation
     */
    static TranslationCoefs centralizedTranslation(const MatF &vector);

    /**
     * @brief Centralizes given shape
     * @param vector Shape represented as column vector
     */
    static void centralize(MatF &vector);

    /**
     * @brief Centralize all shapes
     * @param vectors Shapes represented as column vectors
     */
    static void centralize(std::vector<MatF> &vectors);

    /**
     * @brief Calculates mean of given shapes
     * @param vectors Shapes represented as column vectors
     * @return Mean shape
     */
    static MatF getMeanShape(const std::vector<MatF> &vectors);
};

#endif // PROCRUSTES_H
