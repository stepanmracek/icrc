#ifndef POINTTRACKER_H
#define POINTTRACKER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <list>

#include "linalg/spline.h"
#include "linalg/backprojectionbase.h"
#include "pointtrackerbase.h"
#include "pointtrackeropticalflow.h"
#include "shapenormalizer.h"
#include "videodatabase.h"
#include "imagefilter.h"
#include "strainresultprocessing.h"
#include "strain.h"
#include "longitudinalstrain.h"
#include "coordsystem.h"

class ShapeTracker : public QObject
{
    Q_OBJECT

private:
    Strain *strain;
    PointTrackerBase *pointTracker;
    StrainResultProcessingBase *resultProcessing;
    ListOfImageProcessing frameProcessing;

    void applyProcessing(Mat8 &frame);

public:

    VectorF weights;

    ShapeTracker(Strain *strain,
                 ListOfImageProcessing &frameProcessing,
                 PointTrackerBase *pointTracker,
                 StrainResultProcessingBase *resultProcessing,
                 VectorF weights,
                 QObject *parent = 0)
        : QObject(parent),
          strain(strain),
          pointTracker(pointTracker),
          resultProcessing(resultProcessing),
          weights(weights)
    {
        pointTracker->setParent(this);
        resultProcessing->setParent(this);
        strain->setParent(this);
        addFilters(frameProcessing);
    }

    Strain *getStrain() { return strain; }
    PointTrackerBase *getPointTracker() { return pointTracker; }
    StrainResultProcessingBase *getResultProcessing() { return resultProcessing; }

    void setStrain(Strain *strain)
    {
        delete this->strain;
        this->strain = strain;
        strain->setParent(this);
    }

    void setPointTracker(PointTrackerBase *pointTracker)
    {
        delete this->pointTracker;
        this->pointTracker = pointTracker;
        pointTracker->setParent(this);
    }

    QString getInfo();

    void clearFrameProcessing();

    void addFilters(QList<ImageFilterBase*> newFilters);


    Points track(VectorOfImages &prevFrames, VectorOfShapes &prevShapes, Mat8 &nextFrame, CoordSystemBase *coordSystem);

    //static Points track(Mat8 &prevFrame, Points &prevPoints, Mat8 &nextFrame, Strain &strain, CoordSystemBase *coordSystem, ListOfImageProcessing &frameProcessing, PointTrackerBase &pointTracker);

    /*static VectorOfShapes track(VideoDataBase &data, Strain &strain, ListOfProcessing &frameProcessing,
                                PointTrackerBase &pointTracker, StrainResultProcessingBase &resultProcessing,
                                Points &initialPoints, bool showVideo = false);*/

    static ShapeTracker *getDummyTracker();
};

#endif // POINTTRACKER_H