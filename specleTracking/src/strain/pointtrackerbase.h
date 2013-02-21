#ifndef POINTTRACKERBASE_H
#define POINTTRACKERBASE_H

#include <QObject>

#include "linalg/common.h"

class PointTrackerBase : public QObject
{
    Q_OBJECT

private:
    Points averageResults(VectorOfShapes &results, const VectorF &weights);

protected:
    virtual bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints) = 0;

public:
    PointTrackerBase(QObject *parent = 0) : QObject(parent) { }

    bool track(std::vector<Mat8> &prevFrames, const VectorF &weights,
               Mat8 &nextFrame, VectorOfShapes &prevShapes, Points &nextShape);
};

#endif // POINTTRACKERBASE_H
