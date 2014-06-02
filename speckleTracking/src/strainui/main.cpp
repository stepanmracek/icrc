#include <QString>
#include <QDebug>
#include <QApplication>
#include <vector>

#include "../linalg/common.h"
#include "../linalg/serialization.h"
#include "../linalg/frequencymodulation.h"
#include "../strain/shapenormalizer.h"
#include "../strain/strain.h"
#include "../strain/longitudinalstrain.h"
#include "../strain/imagefilter.h"
#include "../strain/shapetracker.h"
#include "windowanotationmanager.h"

int main(int argc, char *argv[])
{
    PCA *pca = new PCA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, 0, 0);

    strain->deserialize("/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10");

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    qDebug() << "Tracker initializated";

    // create GUI
    QApplication app(argc, argv);
    WindowAnotationManager w("/home/stepo/Dropbox/projekty/icrc/test3/", tracker);
    w.show();
    return app.exec();
}
