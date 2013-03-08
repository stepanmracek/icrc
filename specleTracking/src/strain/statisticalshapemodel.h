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

class StatisticalShapeModel : public QObject
{
    Q_OBJECT

private:
    BackProjectionBase * model;

public:
    StatisticalShapeModel(BackProjectionBase *modelToLearn, VectorOfShapes &shapes, QObject *parent = 0);
    StatisticalShapeModel(BackProjectionBase *learnedModel, QObject *parent = 0);

    void setModel(BackProjectionBase *model)
    {
        if (this->model) delete this->model;
        this->model = model;
    }

    BackProjectionBase *getBackProjectionBase() const
    {
        return this->model;
    }

    Points normalize(Points &input);
    Points iterativeNormalize(Points &input);

    static void showStatisticalShape(BackProjectionBase *model);
};

#endif /* STATISTICALSHAPEMODEL_H_ */
