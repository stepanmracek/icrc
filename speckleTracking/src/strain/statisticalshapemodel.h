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

class StatisticalShapeModel : public SerializableObject
{
    Q_OBJECT

private:
    BackProjectionBase * backProjection;

public:
    StatisticalShapeModel(BackProjectionBase *backProjectionToLearn, VectorOfShapes &shapes, QObject *parent = 0);
    StatisticalShapeModel(BackProjectionBase *learnedBackProjection, QObject *parent = 0);

    void setBackProjection(BackProjectionBase *model)
    {
        if (this->backProjection) delete this->backProjection;
        this->backProjection = model;
        model->setParent(this);
    }

    BackProjectionBase *getBackProjection() const
    {
        return this->backProjection;
    }

    Points normalize(const Points &input);
    Points iterativeNormalize(const Points &input);

    void serialize(cv::FileStorage &storage) const;
    void deserialize(cv::FileStorage &storage);

    static void showStatisticalShape(BackProjectionBase *backProjection);
};

#endif /* STATISTICALSHAPEMODEL_H_ */
