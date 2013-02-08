/*
 * statisticalshapemodel.h
 *
 *  Created on: 9.6.2012
 *      Author: stepo
 */

#ifndef STATISTICALSHAPEMODEL_H_
#define STATISTICALSHAPEMODEL_H_

#include <list>

#include "linalg/common.h"
#include "linalg/pca.h"
//#include "strain/coordsystem.h"

class StatisticalShapeModel
{
public:
    BackProjectionBase &model;

    StatisticalShapeModel(BackProjectionBase &modelToLearn, VectorOfShapes &shapes);
    StatisticalShapeModel(BackProjectionBase &learnedModel) : model(learnedModel) {}

    Points normalize(Points &input);
    Points iterativeNormalize(Points &input);

    static void showStatisticalShape(BackProjectionBase &model);
};

#endif /* STATISTICALSHAPEMODEL_H_ */
