#ifndef STRAINCLASSIFIER_H
#define STRAINCLASSIFIER_H

#include <opencv2/opencv.hpp>

#include "linalg/common.h"

class StrainClassifier
{
public:
    virtual ~StrainClassifier() {}

    virtual void learn(const QVector<VectorF> &positive, const QVector<VectorF> &negative) = 0;
    virtual bool classify(const VectorF &input) = 0;
};

class StrainClassifierSVM : public StrainClassifier
{
private:
    cv::Ptr<cv::SVM> svm;
    cv::Ptr<cv::PCA> pca;

public:
    StrainClassifierSVM();
    void learn(const QVector<VectorF> &positive, const QVector<VectorF> &negative);
    bool classify(const VectorF &input);
};

#endif // STRAINCLASSIFIER_H
