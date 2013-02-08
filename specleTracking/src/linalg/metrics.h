#ifndef METRICS_H
#define METRICS_H

#include <vector>
#include <cassert>
#include <cmath>

#include "vecf.h"
#include "common.h"

class Metrics
{
public:
    Metrics() {}

    virtual float distance(MatF &v1, MatF &v2) = 0;

    virtual ~Metrics() {}
};

class WeightedMetric : public Metrics
{
public:
    MatF w;

    virtual float distance(MatF&v1, MatF&v2) = 0;

    void normalizeWeights()
    {
        normalizeWeights(w);
    }

    static void normalizeWeights(MatF &w)
    {
        int n = w.rows;
        assert(n >= 1);
        assert(w.cols == 1);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            float val = w(i);
            if (val < 0.0)
            {
                val = 0.0;
                w(i) = 0.0;
            }
            sum += val;
        }

        for (int i = 0; i < n; i++)
            w(i) = w(i)/sum*n;
    }

    virtual ~WeightedMetric() {}
};

class EuclideanMetric : public Metrics
{
public:
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

class EuclideanWeightedMetric : public WeightedMetric
{
public:
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

class CityblockMetric : public Metrics
{
public:
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
public:
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
public:
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
public:
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
public:
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
public:
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
public:
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
