/*
 * statisticalshapemodel.cpp
 *
 *  Created on: 9.6.2012
 *      Author: stepo
 */

#include "statisticalshapemodel.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <list>

#include "linalg/common.h"
#include "linalg/spline.h"
#include "linalg/pca.h"
#include "linalg/procrustes.h"
#include "linalg/vecf.h"

StatisticalShapeModel::StatisticalShapeModel(BackProjectionBase *backProjectionToLearn, VectorOfShapes &shapes, QObject *parent) :
    SerializableObject(parent)
{
    backProjection = 0;
    setBackProjection(backProjectionToLearn);

    if (shapes.size() <= 2)
    {
        std::cout << "Not enough samples to learn" << std::endl;
        return;
    }

    std::vector<MatF> matricies;
    for(VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &shape = (*it);
        matricies.push_back(Common::pointsToMatF(shape));
    }

    Procrustes::procrustesAnalysis(matricies, true, 1e-20, 10000);
    backProjection->learn(matricies);
}

StatisticalShapeModel::StatisticalShapeModel(BackProjectionBase *learnedBackProjection, QObject *parent) :
    SerializableObject(parent)
{
    backProjection = 0;
    setBackProjection(learnedBackProjection);
}

void StatisticalShapeModel::serialize(cv::FileStorage &storage) const
{
    backProjection->serialize(storage);
}

void StatisticalShapeModel::deserialize(cv::FileStorage &storage)
{
    backProjection->deserialize(storage);
}

Points StatisticalShapeModel::iterativeNormalize(const Points &input)
{
    MatF inputMat = Common::pointsToMatF(input);
    MatF meanMat = backProjection->getMean();
    assert (inputMat.rows == meanMat.rows);

    // initialize the model parametres to zeros
    MatF params = MatF::zeros(backProjection->getModes(), 1);
    MatF prevParams = MatF::zeros(backProjection->getModes(), 1);

    TranslationCoefs translation(0, 0);
    ScaleAndRotateCoefs rotAndScale(1, 0);
    MatF instance;

    bool end = false;
    int iteration = 1;
    while (!end)
    {
        // generate shape instance
        instance = backProjection->backProject(params);

        MatF inputClone = inputMat.clone();
        translation = Procrustes::centralizedTranslation(inputClone).inv();

        Procrustes::centralize(inputClone);
        rotAndScale = Procrustes::align(instance, inputClone);
        ScaleAndRotateCoefs invRotAndScale = rotAndScale.inv();

        // project input into model coordinate frame
        Procrustes::rotateAndScale(inputClone, invRotAndScale);
        //VecF::mul(inputClone, VecF::dot(inputClone, meanMat));

        // update model parametres
        params = backProjection->project(inputClone);

        // 3*sigma normalization
        backProjection->threeSigmaNormalization(params);

        // finished?
        MatF paramsDiff = prevParams-params;
        float diff = VecF::dot(paramsDiff, paramsDiff);
        if (diff <= 1e-10) end = true;
        prevParams = params.clone();

        iteration++;
    }

    Procrustes::rotateAndScale(instance, rotAndScale);
    Procrustes::translate(instance, translation);
    Points result = Common::matFToPoints(instance);
    return result;
}

Points StatisticalShapeModel::normalize(const Points &input)
{
    MatF inputMat = Common::pointsToMatF(input);
    MatF meanMat = backProjection->getMean();
    assert (inputMat.rows == meanMat.rows);

    // Align to the mean shape
    TranslationCoefs translateC =  Procrustes::centralizedTranslation(inputMat);
    Procrustes::translate(inputMat, translateC);
    ScaleAndRotateCoefs rotAndScaleC = Procrustes::align(inputMat, meanMat);
    Procrustes::rotateAndScale(inputMat, rotAndScaleC);

    // 3*sigma normalization
    MatF params = backProjection->project(inputMat);
    backProjection->threeSigmaNormalization(params);

    // Back-projection
    MatF normalizedShapeMat = backProjection->backProject(params);

    // Inverse transform
    ScaleAndRotateCoefs invRotAndScaleC = rotAndScaleC.inv();
    Procrustes::rotateAndScale(normalizedShapeMat, invRotAndScaleC);
    TranslationCoefs invTranslateC = translateC.inv();
    Procrustes::translate(normalizedShapeMat, invTranslateC);

    Points result = Common::matFToPoints(normalizedShapeMat);
    return result;
}

struct SSMBackprojectData
{
    SSMBackprojectData(BackProjectionBase *model) : model(model)
	{
        parameters = MatF::zeros(model->getModes(), 1);
		winname = "shape";
	}

	std::string winname;
	int trackbarValues[10];
	MatF parameters;
    BackProjectionBase *model;
	Spline spline;
};

void drawShape(SSMBackprojectData &data)
{
	Mat8 white = Mat8::ones(300,300)*255;
    MatF shapeMat = data.model->backProject(data.parameters);

    ScaleAndRotateCoefs rot(300, 0);
	Procrustes::rotateAndScale(shapeMat, rot);
    TranslationCoefs trans(150, 150);
	Procrustes::translate(shapeMat, trans);

	Points shape = Common::matFToPoints(shapeMat);
    //data.spline.drawSpline(shape, white, true, 0);
    int n = shape.size();
    for (int i = 0; i < n; i++)
    {
        white(shape.at(i)) = 0;
    }

	cv::imshow(data.winname, white);
}

void onParameterChange(int, void* param)
{
    SSMBackprojectData *data = (SSMBackprojectData*)param;
    for (int i = 0; i < 10; i++)
    {
    	std::stringstream ss;
    	ss << i;
    	std::string tbName = ss.str();
        float value = cv::getTrackbarPos(tbName, data->winname);
        value = (value-5.0)/5.0 * (3*sqrt(data->model->getVariance(i)));
        data->parameters(i) = value;
    }

    drawShape(*data);
}

void StatisticalShapeModel::showStatisticalShape(BackProjectionBase *model)
{
	std::cout << "shape loaded" << std::endl;

    SSMBackprojectData data(model);
	cv::namedWindow(data.winname);
	for (int i = 0; i < 10; i++)
	{
		std::stringstream ss;
		ss << i;
		std::string tbName = ss.str();
		data.trackbarValues[i] = 5;
		cv::createTrackbar(tbName, data.winname, &(data.trackbarValues[i]), 10, onParameterChange, &data);
	}
	drawShape(data);

	while (1)
    {
        char key = cv::waitKey(0);
        if (key == 27) break;
    }
}
