#include "vecf.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <vector>
#include <cassert>

float VecF::sqrMagnitude(MatF &vector)
{
    int n = vector.rows;
    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        float v = vector(i);
        sum +=  (v * v);
    }
    return sum;
}

float VecF::magnitude(MatF &vector)
{
    return sqrt(sqrMagnitude(vector));
}

float VecF::dot(MatF &v1, MatF &v2)
{
    int n = v1.rows;
    assert(n == v2.rows);
    float sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += (v1(i) * v2(i));
    }
    return sum;
}

MatF & VecF::normalize(MatF &vector)
{
    int n = vector.rows;

    float mag = magnitude(vector);
    for (int i = 0; i < n; i++)
    {
        float v = vector(i);
        vector(i) = v/mag;
    }

    return vector;
}

MatF VecF::normalized(MatF &vector)
{
    int n = vector.rows;
    float mag = magnitude(vector);
    MatF newVector(vector.rows, vector.cols, CV_64F);
    for (int i = 0; i < n; i++)
    {
        float v = vector(i);
        newVector(i) = v/mag;
    }

    return newVector;
}

MatF & VecF::mul(MatF &vector, float value)
{
    int n = vector.rows;

    for (int i = 0; i < n; i++)
    {
        float v = vector(i ,0);
        vector(i) = v*value;
    }

    return vector;
}

bool VecF::isZero(MatF &vector)
{
    int n = vector.rows;
    for (int i = 0; i < n; i++)
    {
        if (vector(i) != 0) return false;
    }
    return true;
}

void VecF::toFile(VectorF &vector, const QString &path, bool append)
{
    QFile f(path);
    if (append)
        f.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        f.open(QIODevice::WriteOnly);
    QTextStream out(&f);

    int n = vector.size();
    for (int i = 0; i < n; i++)
    {
        out << vector[i];
        out << '\n';
    }
    out << '\n';

    out.flush();
    f.close();
}

VectorF VecF::fromFile(const QString &path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);

    VectorF result;
    while (!in.atEnd())
    {
        float value;
        in >> value;

        if (in.status() == QTextStream::ReadPastEnd)
            break;

        result.push_back(value);
    }

    return result;
}

int VecF::maxIndex(MatF &vector)
{
    double min, max;
    int minIdx, maxIdx;
    cv::minMaxIdx(vector, &min, &max, &minIdx, &maxIdx);
    return maxIdx;
}

int VecF::maxIndex(VectorF &vector)
{
    return std::max_element(vector.begin(), vector.end()) - vector.begin();
}

float VecF::maxValue(MatF &vector)
{
    double min, max;
    cv::minMaxIdx(vector, &min, &max);
    return max;
}

float VecF::maxValue(VectorF &vector)
{
    return *std::max_element(vector.begin(), vector.end());
}

int VecF::minIndex(MatF &vector)
{
    double min, max;
    int minIdx, maxIdx;
    cv::minMaxIdx(vector, &min, &max, &minIdx, &maxIdx);
    return minIdx;
}

int VecF::minIndex(VectorF &vector)
{
    return std::min_element(vector.begin(), vector.end()) - vector.begin();
}

float VecF::minValue(MatF &vector)
{
    double min, max;
    cv::minMaxIdx(vector, &min, &max);
    return min;
}

float VecF::minValue(VectorF &vector)
{
    return *std::min_element(vector.begin(), vector.end());
}

float VecF::meanValue(MatF &vector)
{
    return cv::sum(vector)[0] / vector.rows;
}

float VecF::meanValue(const VectorF &vector)
{
    return std::accumulate(vector.begin(), vector.end(), 0.0f) / vector.size();
}

float VecF::stdDeviation(MatF &vector)
{
    float mean = meanValue(vector);
    float sum = 0;
    int r = vector.rows;
    for (int i = 0; i < r; i++)
    {
        sum += ((vector(i) - mean)*(vector(i) - mean));
    }

    return sqrt((1.0/(r - 1.0)) * sum);
}

float VecF::stdDeviation(VectorF &vector)
{
    float mean = meanValue(vector);
    float sum = 0;
    int r = vector.size();
    for (int i = 0; i < r; i++)
    {
        sum += ((vector.at(i) - mean)*(vector.at(i) - mean));
    }

    return sqrt((1.0/(r - 1.0)) * sum);
}

MatF VecF::fromVector(VectorF &vec)
{
    int r = vec.size();
    MatF result = MatF::zeros(r, 1);
    for (int i = 0; i < r; i++)
    {
        result(i) = vec.at(i);
    }
    return result;
}

VectorF VecF::toVector(MatF &vector)
{
    VectorF result;
    for (int i = 0; i < vector.rows; i++)
        result.push_back(vector(i));
    return result;
}

MatF VecF::normalizeComponents(MatF &vector, VectorF &minValues, VectorF &maxValues, bool fixedBounds)
{
    int r = vector.rows;
    MatF result = MatF::zeros(r, 1);
    for (int i = 0; i < r; i++)
    {
        float val = vector(i);

        if (fixedBounds)
        {
            if (val > maxValues[i]) val = maxValues[i];
            if (val < minValues[i]) val = minValues[i];
        }

        val = (val - minValues[i])/(maxValues[i]-minValues[i]);
        result(i) = val;
    }
    return result;
}

MatF VecF::normalizeComponents(MatF &vector)
{
    float min = minValue(vector);
    float max = maxValue(vector);

    MatF result = (vector-min)/(max-min);
    return result;
}

MatF VecF::meanVector(std::vector<MatF> &vectors)
{
    MatF result = MatF::zeros(vectors[0].rows, vectors[0].cols);
    int n = vectors.size();
    for (int i = 0; i < n; i++)
        result += vectors[i];

    result = result / ((double)n);
    return result;
}

VectorF VecF::resample(const VectorF &vector, int samplesCount)
{
    if (vector.size() == samplesCount)
    {
        return vector;
    }

    VectorF result;
    for (int i = 0; i < samplesCount; i++)
    {
        float indexF = (vector.size()-1) * i / ((float)(samplesCount));
        unsigned int indexI = floor(indexF);
        float delta = indexF - indexI;

        if ((indexI + 1) == vector.size()) break;
        //qDebug() << statIndex << indexF << indexI;

        float first = vector[indexI];
        float second = vector[indexI + 1];
        float interpolated = first + delta * (second - first);

        result.push_back(interpolated);
    }

    return result;
}

VectorF VecF::floatingAverage(const VectorF &input, int kernelSize)
{
    assert(kernelSize >= 1);
    assert(kernelSize % 2 == 1);
    int w = kernelSize/2;

    size_t n = input.size();
    VectorF result(n);
    for (size_t i = 0; i < n; i++)
    {
        int min = i - w;
        if (min < 0) min = 0;
        size_t max = i + w;
        if (max >= n) max = n - 1;
        int count = 0;
        float sum = 0.0f;
        for (size_t j = min; j <= max; j++)
        {
           sum += input[j];
           count++;
        }
        result[i] = sum/count;
    }

    return result;
}
