#include "strainclassifier.h"

#include <QVector>
#include <QDebug>

StrainClassifierSVM::StrainClassifierSVM()
{
    svm = new cv::SVM();
    pca = new cv::PCA();
}

void StrainClassifierSVM::learn(const QVector<VectorF> &positive, const QVector<VectorF> &negative)
{
    qDebug() << "SVM learn";
    int cols = positive[0].size();
    int dataSize = positive.count() + negative.count();
    MatF data(dataSize, cols);
    MatF responses(dataSize, 1);

    qDebug() << "  dataSize" << dataSize;

    for (int r = 0; r < positive.count(); r++)
    {
        for (int c = 0; c < cols; c++)
        {
            data(r, c) = positive[r][c];
            responses(r) = 1;
        }
    }
    int shift = positive.count();
    for (int r = shift; r < negative.count() + shift; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            data(r, c) = negative[r-shift][c];
            responses(r) = -1;
        }
    }

    (*pca)(data, cv::Mat(), CV_PCA_DATA_AS_ROW);
    pca->eigenvectors = pca->eigenvectors.rowRange(0, 10);
    pca->eigenvalues = pca->eigenvalues.rowRange(0, 10);
    MatF projectedData(dataSize, 10);

    for (int r = 0; r < positive.count(); r++)
    {
        MatF example(cols, 1);
        for (int c = 0; c < cols; c++)
        {
            example(c) = positive[r][c];
        }
        projectedData.row(r) = pca->project(example.t());
    }
    shift = positive.count();
    for (int r = shift; r < negative.count() + shift; r++)
    {
        MatF example(cols, 1);
        for (int c = 0; c < cols; c++)
        {
            example(c) = negative[r-shift][c];
        }
        projectedData.row(r) = pca->project(example.t());
    }

    cv::SVMParams params;
    params.svm_type = cv::SVM::C_SVC;
    params.kernel_type = cv::SVM::RBF;
    params.term_crit   = cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 1e-6);

    svm->train(projectedData, responses, cv::Mat(), cv::Mat(), params);
}

bool StrainClassifierSVM::classify(const VectorF &input)
{
    int rows = input.size();
    MatF inputMatrix(rows, 1);
    for (int i = 0; i < rows; i++) inputMatrix(i) = input[i];
    return svm->predict(pca->project(inputMatrix.t())) > 0;
}
