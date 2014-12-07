#include "pca.h"

PCA::PCA(std::vector<MatF> &vectors, int maxComponents, QObject *parent) : BackProjectionBase(parent)
{
    learn(vectors, maxComponents);
}

PCA::PCA(const QString &path, QObject *parent) : BackProjectionBase(parent)
{
    cv::FileStorage storage(path.toStdString(), cv::FileStorage::READ);
    deserialize(storage);
}

void PCA::learn(const std::vector<MatF> &vectors, int maxComponents)
{
    MatF data = Common::columnVectorsToDataMatrix(vectors);
    cvPca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_COL, maxComponents);
}

void PCA::serialize(cv::FileStorage &storage) const
{
    storage << "eigenvalues" << cvPca.eigenvalues;
    storage << "eigenvectors" << cvPca.eigenvectors;
    storage << "mean" << cvPca.mean;
}

void PCA::deserialize(cv::FileStorage &storage)
{
    storage["eigenvalues"] >> cvPca.eigenvalues;
    storage["eigenvectors"] >> cvPca.eigenvectors;
    storage["mean"] >> cvPca.mean;
}

MatF PCA::project(const MatF &in)
{
    MatF out = cvPca.project(in);
    return out;
}

MatF PCA::backProject(const MatF &in)
{
    MatF out = cvPca.backProject(in);
    return out;
}

void PCA::setModes(int modes)
{
    if (modes >= getModes()) return;
    cvPca.eigenvectors = cvPca.eigenvectors.rowRange(0, modes);
    cvPca.eigenvalues = cvPca.eigenvalues.rowRange(0, modes);
}

float PCA::getEigenVal(int mode)
{
    float v = cvPca.eigenvalues.at<float>(mode);
    return v < 0 ? 0 : v;
}

void PCA::modesSelectionThreshold(float t)
{
	if (t >= 1) return; // nothing to do here

    float sum = 0.0;
    int r;
    for (r = 0; r < cvPca.eigenvalues.rows; r++)
    {
        sum += getEigenVal(r);
    }

    float actualSum = 0.0;
    for (r = 0; r < cvPca.eigenvalues.rows; r++)
    {
        actualSum += getEigenVal(r);

        if (actualSum/sum > t)
            break;
    }

    setModes(r+1);
}

float PCA::getVariance(int mode)
{
    return getEigenVal(mode);
}

MatF PCA::getMean()
{
    MatF mean(cvPca.mean);
    return mean;
}
