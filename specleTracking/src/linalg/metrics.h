#ifndef METRICS_H
#define METRICS_H

#include <QObject>
#include <vector>
#include <cassert>
#include <cmath>

#include "vecf.h"
#include "common.h"

/**
 * @brief The Metrics abstract class provides common interface for all used metrics (Euclidean, city-block,...)
 */
class Metrics : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Basic constructor
     * @param parent Parent object in Qt hierarchy
     */
    Metrics(QObject *parent = 0) : QObject(parent) { }

    /**
     * @brief The distance between two input vectors
     * @param v1 First vector (column matrix)
     * @param v2 Second vector (column matrix)
     * @return Returns the distance
     */
    virtual float distance(MatF &v1, MatF &v2) = 0;

    virtual ~Metrics() {}
};

/**
 * @brief The weighted metric abstract class
 */
class WeightedMetric : public Metrics
{
    Q_OBJECT

public:
    /**
     * @brief WeightedMetric constructor
     * @param parent Parent object in Qt hierarchy
     */
    WeightedMetric(QObject *parent = 0) : Metrics(parent) { }

    /**
     * @brief Weights
     */
    MatF w;

    virtual float distance(MatF&v1, MatF&v2) = 0;

    /**
     * @brief Normalize weights, such that sum of the components is 1
     */
    void normalizeWeights()
    {
        Common::normalizeSumOne(w);
    }

    virtual ~WeightedMetric() {}
};

/**
 * @brief The Euclidean metric
 */
class EuclideanMetric : public Metrics
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    EuclideanMetric(QObject *parent = 0) : Metrics(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            float v = (v1(i, 0) - v2(i, 0));
            sum += v*v;
        }

        return sqrt(sum);
    }

    virtual ~EuclideanMetric() {}
};

/**
 * @brief The Euclidean weighted metric
 */
class EuclideanWeightedMetric : public WeightedMetric
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    EuclideanWeightedMetric(QObject *parent) : WeightedMetric(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);
        assert(w.rows >= n);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            float v = w(i) * (v1(i) - v2(i));
            sum += v*v;
        }

        return sqrt(sum);
    }

    virtual ~EuclideanWeightedMetric() {}
};

/**
 * @brief The CityblockMetric class
 */
class CityblockMetric : public Metrics
{
    Q_OBJECT

public:
    /**
     * @brief CityblockMetric Constructor
     * @param parent Parent object in Qt hieararchy
     */
    CityblockMetric(QObject *parent = 0) : Metrics(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            sum += fabs(v1(i, 0) - v2(i, 0));
        }

        return sum;
    }

    virtual ~CityblockMetric() {}
};

class CityblockWeightedMetric : public WeightedMetric
{
    Q_OBJECT

public:
    CityblockWeightedMetric(QObject *parent = 0) : WeightedMetric(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);
        assert(w.rows >= n);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            sum += w(i) * fabs(v1(i, 0) - v2(i, 0));
        }

        return sum;
    }

    virtual ~CityblockWeightedMetric() {}
};

class CorrelationMetric : public Metrics
{
    Q_OBJECT

public:
    CorrelationMetric(QObject *parent = 0) : Metrics(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        return 1.0 - correlation(v1, v2);
    }

    float correlation(MatF&v1, MatF&v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        float mean1 = VecF::meanValue(v1);
        float mean2 = VecF::meanValue(v2);
        float std1 = VecF::stdDeviation(v1);
        float std2 = VecF::stdDeviation(v2);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            sum += ((v1(i) - mean1)/std1)*((v2(i) - mean2)/std2);
        }

        return (1.0/(n-1.0)) * sum;
    }

    virtual ~CorrelationMetric() {}
};

class CorrelationWeightedMetric : public WeightedMetric
{
    Q_OBJECT

public:
    CorrelationWeightedMetric(QObject *parent = 0) : WeightedMetric(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        return 1.0 - correlation(v1, v2);
    }

    float correlation(MatF&v1, MatF&v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        MatF v1w = v1.mul(w);
        MatF v2w = v2.mul(w);

        float mean1 = VecF::meanValue(v1w);
        float mean2 = VecF::meanValue(v2w);
        float std1 = VecF::stdDeviation(v1w);
        float std2 = VecF::stdDeviation(v2w);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            sum += ((v1w(i) - mean1)/std1)*((v2w(i) - mean2)/std2);
        }

        return (1.0/(n-1.0)) * sum;
    }

    virtual ~CorrelationWeightedMetric() {}
};

class CosineMetric : public Metrics
{
    Q_OBJECT

public:
    CosineMetric(QObject *parent = 0) : Metrics(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        MatF mul = (v1.t()*v2);
        float dist = 1.0 - (mul(0))/(VecF::magnitude(v1) * VecF::magnitude(v2));

        if(dist < 0.0)
        {
        	// qDebug() << "CosineMetric" << dist;
        	dist = 0.0;
        }

        return dist;
    }

    virtual ~CosineMetric() {}
};

class CosineWeightedMetric : public WeightedMetric
{
    Q_OBJECT

public:
    CosineWeightedMetric(QObject *parent = 0) : WeightedMetric(parent) { }

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);

        MatF v1w = v1.mul(w);
        MatF v2w = v2.mul(w);

        MatF mul = (v1w.t()*v2w);
        float dist = 1.0 - (mul(0))/(VecF::magnitude(v1w) * VecF::magnitude(v2w));

        if(dist < 0.0)
        {
            // qDebug() << "CosineWeightedMetric" << dist;
            dist = 0.0;
        }

        return dist;
    }

    virtual ~CosineWeightedMetric() {}
};

/*class MahalanobisMetric : public Metrics
{
public:
    MatF invCov;
    MatF mean;

    MahalanobisMetric() {}

    MahalanobisMetric(MatF&invCov, MatF&mean) : invCov(invCov), mean(mean) {}

    MahalanobisMetric(QVector<Matrix> &samples);

    void learn(QVector<Matrix> &samples);

    virtual float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);
        assert(v1.cols == 1);
        assert(v2.cols == 1);

        return cv::Mahalonobis(v1, v2, invCov);
    }

    float distance(MatF &v)
    {
        //qDebug() << "mean:" << mean.rows << mean.cols << "v:" << v.rows << v.cols << "invCov" << invCov.rows << invCov.cols;
        return cv::Mahalanobis(v, mean, invCov);
    }

    virtual ~MahalanobisMetric() {}
};

class MahalanobisWeightedMetric : public WeightedMetric
{
public:
    MatF invCov;
    MatF mean;

    MahalanobisWeightedMetric() {}

    MahalanobisWeightedMetric(MatF &invCov, MatF &mean) : invCov(invCov), mean(mean) {}

    MahalanobisWeightedMetric(QVector<Matrix> &samples);

    virtual float distance(MatF&v1, MatF&v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);
        assert(v1.cols == 1);
        assert(v2.cols == 1);

        return cv::Mahalonobis(v1.mul(w), v2.mul(w), invCov);
    }

    float distance(MatF&v)
    {
        return cv::Mahalanobis(v.mul(w), mean.mul(w), invCov);
    }

    virtual ~MahalanobisWeightedMetric() {}
};*/

class SumOfSquareDifferences : public Metrics
{
    Q_OBJECT

public:
    SumOfSquareDifferences(QObject *parent = 0) : Metrics(parent) { }

    float distance(MatF &v1, MatF &v2)
    {
        int n = v1.rows;
        assert(n == v2.rows);
        assert(v1.cols == 1);
        assert(v2.cols == 1);

        float sum = 0.0;
        for (int i = 0; i < n;i++)
        {
            float d = (v1(i) - v2(i));
            sum += d*d;
        }
        return sum;
    }

    virtual ~SumOfSquareDifferences() {}
};

#endif // METRICS_H
