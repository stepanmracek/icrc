#include "vecf.h"

#include <QFile>
#include <QTextStream>
#include <vector>
#include <cassert>

/*MatF VecF::fromFile(const QString &path)
{
	assert(QFile::exists(path));
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);

    VectorF data;
    while (!in.atEnd())
    {
        double value;
        in >> value;

        if (in.status() == QTextStream::ReadPastEnd)
            break;

        data.append(value);
    }

    f.close();

    int r = data.size();
    MatF vector(r, 1, CV_64F);
    for (int i = 0; i < r; i++)
    {
        vector(i) = data.at(i);
    }

    if(Common::MatFContainsNan(vector))
    {
    	qDebug() << path.toStdString().c_str() << "contains Nan";
    }
    return vector;
}*/

double VecF::sqrMagnitude(MatF &vector)
{
    int n = vector.rows;
    double sum = 0;
    for (int i = 0; i < n; i++)
    {
        double v = vector(i);
        sum +=  (v * v);
    }
    return sum;
}

double VecF::magnitude(MatF &vector)
{
    return sqrt(sqrMagnitude(vector));
}

double VecF::dot(MatF &v1, MatF &v2)
{
    int n = v1.rows;
    assert(n == v2.rows);
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += (v1(i) * v2(i));
    }
    return sum;
}

MatF & VecF::normalize(MatF &vector)
{
    int n = vector.rows;

    double mag = magnitude(vector);
    for (int i = 0; i < n; i++)
    {
        double v = vector(i);
        vector(i) = v/mag;
    }

    return vector;
}

MatF VecF::normalized(MatF &vector)
{
    int n = vector.rows;
    double mag = magnitude(vector);
    MatF newVector(vector.rows, vector.cols, CV_64F);
    for (int i = 0; i < n; i++)
    {
        double v = vector(i);
        newVector(i) = v/mag;
    }

    return newVector;
}

MatF & VecF::mul(MatF &vector, double value)
{
    int n = vector.rows;

    for (int i = 0; i < n; i++)
    {
        double v = vector(i ,0);
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

/*void VecF::toFileWithIndicies(MatF &vector, const QString &path, bool append)
{
    QFile f(path);
    if (append)
        f.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        f.open(QIODevice::WriteOnly);
    QTextStream out(&f);

    int n = vector.rows;
    for (int i = 0; i < n; i++)
    {
        out << i << ' ' << vector(i) << '\n';
    }
    out << '\n';

    out.flush();
    f.close();
}*/

/*void VecF::toFileTwoCols(MatF &vector,const QString &path, bool append)
{
    int n = vector.rows;
    assert(n % 2 == 0);
    n = n/2;

    QFile f(path);
    if (append)
        f.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        f.open(QIODevice::WriteOnly);
    QTextStream out(&f);

    for (int i = 0; i < n; i++)
    {
        out <<vector(i) << ' ' <<  vector(i+n) << '\n';
    }
    out << '\n';

    out.flush();
    f.close();
}*/

/*MatF VecF::fromTwoColsFile(const QString &path)
{
	assert(QFile::exists(path));
    QFile f(path);
    bool opened = f.open(QIODevice::ReadOnly);
    assert(opened);
    QTextStream in(&f);

    VectorF x;
    VectorF y;

    bool isX = true;
    while (!in.atEnd())
    {
        double value;
        in >> value;

        if (in.status() == QTextStream::ReadPastEnd)
            break;

        if (isX)
        {
            x.append(value);
        }
        else
        {
            y.append(value);
        }

        isX = !isX;
    }
    assert(x.size() == y.size());
    f.close();

    int r = x.size() + y.size();
    MatF vector(r, 1, CV_64F);

    for (int i = 0; i < x.size(); i++)
    {
        vector(i) = x[i];
    }
    for (int i = 0; i < y.size(); i++)
    {
        vector(x.size()+i) = y[i];
    }

    return vector;
}*/

int VecF::maxIndex(MatF &vector)
{
    double max = -1e300;
    int index = -1;
    int r = vector.rows;
    assert(r > 0);
    for (int i = 0; i < r; i++)
    {
        double v = vector(i);
        if (v > max)
        {
            index = i;
            max = v;
        }
    }
    return index;
}

int VecF::maxIndex(VectorF &vector)
{
    double max = -1e300;
    int index = -1;
    int r = vector.size();
    assert(r > 0);
    for (int i = 0; i < r; i++)
    {
        double v = vector.at(i);
        if (v > max)
        {
            index = i;
            max = v;
        }
    }
    assert(index != -1);
    /*if (index == -1)
    {
    	std::cout << vector;
    }*/
    return index;
}

double VecF::maxValue(MatF &vector)
{
    return vector(maxIndex(vector));
}

double VecF::maxValue(VectorF &vector)
{
    return vector.at(maxIndex(vector));
}

int VecF::minIndex(MatF &vector)
{
    double min = 1e300;
    int index = -1;
    int r = vector.rows;
    assert(r > 0);
    for (int i = 0; i < r; i++)
    {
        double v = vector(i);
        if (v < min)
        {
            min = v;
            index = i;
        }
    }
    return index;
}

int VecF::minIndex(VectorF &vector)
{
    double min = 1e300;
    int index = -1;
    int r = vector.size();
    assert(r > 0);
    for (int i = 0; i < r; i++)
    {
        double v = vector.at(i);
        if (v < min)
        {
            min = v;
            index = i;
        }
    }
    assert(index != -1);
    /*if (index == -1)
    {
    	std::cout << vector;
    }*/
    return index;
}

double VecF::minValue(MatF &vector)
{
    return vector(minIndex(vector));
}

double VecF::minValue(VectorF &vector)
{
	int i = minIndex(vector);
    return vector.at(i);
}

double VecF::meanValue(MatF &vector)
{
    double sum = 0;
    int r = vector.rows;
    for (int i = 0; i < r; i++)
    {
        sum += vector(i);
    }

    return sum/r;
}

double VecF::meanValue(const VectorF &vector)
{
    double sum = 0;
    int r = vector.size();
    for (int i = 0; i < r; i++)
    {
        sum += vector.at(i);
    }

    return sum/r;
}

double VecF::stdDeviation(MatF &vector)
{
    double mean = meanValue(vector);
    double sum = 0;
    int r = vector.rows;
    for (int i = 0; i < r; i++)
    {
        sum += ((vector(i) - mean)*(vector(i) - mean));
    }

    return sqrt((1.0/(r - 1.0)) * sum);
}

double VecF::stdDeviation(VectorF &vector)
{
    double mean = meanValue(vector);
    double sum = 0;
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
        double val = vector(i);

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
    double min = minValue(vector);
    double max = maxValue(vector);

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
    VectorF result;

    for (int i = 0; i < samplesCount; i++)
    {
        float indexF = (vector.size()-1) * i / ((float)(samplesCount));
        int indexI = floor(indexF);
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
