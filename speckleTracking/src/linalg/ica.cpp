#include "ica.h"

#include <QDebug>

#include "pca.h"
#include "vecf.h"

double g1(double u)
{
    return tanh(u);
}

double g2(double u)
{
    return u*exp(-(u*u)/2.0);
}

void ICA::learn(const std::vector<MatF> &vectors, int independentComponentCount, double eps, int maxIterations, bool debug)
{
    qDebug() << "ICA learn; vectors:" << vectors.size();

    debug = true;
    int n = vectors.size();
    assert(n > 0);
    int m = vectors[0].rows;

    if (independentComponentCount <= 0)
        independentComponentCount = m-1;
    assert(independentComponentCount <= m);

    // mean value
    mean = MatF::zeros(m, 1);
    std::vector<MatF> workingCopy;
    for (int i = 0; i < n; i++)
    {
        workingCopy.push_back(vectors[i].clone());
        mean += vectors[i];
    }
    mean = mean / n;

    for (int i = 0; i < n; i++)
        workingCopy[i] = workingCopy[i] - mean;

    // whitening; x <- E * D^(-1/2) * E^T * x
    // E - eigenvectors
    // D - eigenvalues
    qDebug() << "  PCA whitening";
    PCA pca(workingCopy);

    int eValCount = pca.getModes();
    MatF eVals = MatF::zeros(eValCount, eValCount);
    for (int i = 0; i < eValCount; i++)
    {
        double eval = pca.getEigenVal(i);
        if (eval < 1e-100) eval = 1e-100;
        eVals(i,i) = 1.0/sqrt(eval);
    }

    EDET = pca.cvPca.eigenvectors * eVals * pca.cvPca.eigenvectors.t();
    //EDET = pca.cvPca.eigenvectors.t() * eVals * pca.cvPca.eigenvectors;

    for (int i = 0; i < n; i++)
        workingCopy[i] = EDET * workingCopy[i];
    EDETinv = EDET.inv();

    // FAST ICA
    qDebug() << "  Fast ICA";
    W = MatF::eye(m, independentComponentCount);
    for (int p = 0; p < independentComponentCount; p++)
    {
        double oldErr = 1.0;
        double sameErrCount = 0;

        // initicalization
        MatF w = W.col(p); // Matrix::ones(m, 1, CV_64F);
        w = w/VecF::magnitude(w);

        int iteration = 1;
        while (1)
        {
            // w <- E{x * g(w^T * x)} - E{g'(w^T * x)} * w
            // w <- w / |w|
            MatF expected1 = MatF::zeros(m, 1);
            double expected2 = 0;
            for (int i = 0; i < n; i++)
            {
                MatF wtxMat = w.t()*workingCopy[i];
                double wtx = wtxMat(0);
                expected1 += workingCopy[i] * g1(wtx);
                expected2 += g2(wtx);
            }
            expected1 = expected1/n;
            MatF expected2Mat = expected2/n * w;
            MatF wOld = w.clone();
            w = expected1 - expected2Mat;
            w = w/VecF::magnitude(w);

            assert(!Common::matrixContainsNan(w));

            /*if (p > 0)
            {
                // decorrelate
                MatF sum = MatF::zeros(m, 1);
                for (int j = 0; j < p; j++)
                {
                    sum += W.col(j) * W.col(j).t() * w;
                    w = w-sum;
                    w = w/VecF::magnitude(w);
                }
            }*/

            double err = fabs(1-fabs(VecF::dot(w, wOld)));
            if (err == oldErr) sameErrCount++;
            if (err < eps || iteration > maxIterations || sameErrCount > 10)
            {
                break;
            }
            oldErr = err;
            iteration++;
        }

        if (debug)
            std::cout << "  " << (p+1) << "/" << independentComponentCount << std::endl;
    }
    W = W.t();

    assert(!Common::matrixContainsNan(W));

    //return;

    // decorrelation, repeat until convergence
    if (debug)
        std::cout << "  projection matrix decorrelation" << std::endl;
    MatF oldW;
    MatF diffW;
    W.copyTo(oldW);
    W = W / sqrt(cv::norm(W*W.t()));
    for (int i = 0; i < 10000; i++)
    {
        W = 1.5*W - 0.5*W*W.t()*W;
        diffW = oldW-W;
        double diff = Common::absSum(diffW);

        if (Common::matrixContainsNan(W))
        {
            std::cout << "  W during decorrelation step " << i << " contains NaN" << std::endl;
            oldW.copyTo(W);
            assert(!Common::matrixContainsNan(oldW));
            break;
        }

        /*if (debug)
            std::cout << " decorrelation " << i << " " << diff << std::endl;*/

        if (diff != diff || diff < eps)
            break;

        W.copyTo(oldW);
    }

    if (debug)
        std::cout << "  calculating projected space variance";
    calculateVariances(vectors);

    if (debug)
        std::cout << "ICA done" << std::endl;
}

ICA::ICA(std::vector<MatF> &vectors, int independentComponentCount, double eps, int maxIterations, bool debug, QObject *parent) :
    BackProjectionBase(parent)
{
    learn(vectors, independentComponentCount, eps, maxIterations, debug);
}

ICA::ICA(const QString &path, QObject *parent) : BackProjectionBase(parent)
{
    cv::FileStorage storage(path.toStdString(), cv::FileStorage::READ);
    deserialize(storage);
}

MatF ICA::whiten(const MatF &vector)
{
    MatF result = EDET * (vector - mean);
    return result;
}

MatF ICA::project(const MatF &vector)
{
    MatF whitened = whiten(vector);
    MatF result = W * whitened;
    return result;
}

MatF ICA::backProject(const MatF &vector)
{
    MatF result = W.t() * vector;
    result = EDETinv * result;
    result = result + mean;
    return result;
}

void ICA::serialize(cv::FileStorage &storage) const
{
    storage << "EDET" << EDET;
    storage << "EDETinv" << EDETinv;
    storage << "W" << W;
    storage << "mean" << mean;
    storage << "variances" << variances;
}

void ICA::deserialize(cv::FileStorage &storage)
{
    storage["EDET"] >> EDET;
    storage["EDETinv"] >> EDETinv;
    storage["W"] >> W;
    storage["mean"] >> mean;
    storage["variances"] >> variances;
}

void ICA::calculateVariances(const std::vector<MatF> &vectors)
{
    // we will calculate all projected values per each mode
    int m = getModes();
    VectorF values[m];

    int n = vectors.size();
    for (int i = 0; i < n; i++)
    {
        MatF projected = project(vectors[i]);
        for (int j = 0; j < m; j++)
        {
            values[j].push_back(projected(j));
        }
    }

    variances = MatF::zeros(m, 1);
    for (int j = 0; j < m; j++)
        variances(j) = pow(VecF::stdDeviation(values[j]), 2);
}
