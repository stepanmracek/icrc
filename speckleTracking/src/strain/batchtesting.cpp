#include "batchtesting.h"

#include <QDir>

#include "pointtrackerneighbouropticalflow.h"
#include "shapetracker.h"
#include "longitudinalstrain.h"
#include "shapenormalizer.h"
#include "statisticalshapemodel.h"
#include "linalg/pca.h"

void BatchTesting::process()
{
    // Create Trackers

    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerNeighbourOpticalFlow *pointTracker = new PointTrackerNeighbourOpticalFlow(20, 31, 5);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1);
    ShapeTracker tracker1(strain, processing, pointTracker, postProcessing, weights);
}
