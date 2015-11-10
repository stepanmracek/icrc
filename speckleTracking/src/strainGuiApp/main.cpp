#include <QString>
#include <QDebug>
#include <QApplication>
#include <QFileDialog>
#include <QCommandLineParser>

#include "linalg/common.h"
#include "linalg/serialization.h"
#include "linalg/frequencymodulation.h"
#include "strain/shapenormalizer.h"
#include "strain/strain.h"
#include "strain/longitudinalstrain.h"
#include "strain/imagefilter.h"
#include "strain/shapetracker.h"
#include "strainui/windowanotationmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("directory", "Directory containing video files", "directory"));
    parser.addOption(QCommandLineOption("model", "Left Vebtricle model file", "model"));
    parser.addHelpOption();
    parser.process(app);
    
    QString directory, model;
    if (!parser.isSet("directory"))
        directory = QFileDialog::getExistingDirectory(0, "Select directory containing videos");
    else
        directory = parser.value("directory");
    
    if (!parser.isSet("model"))
        model = QFileDialog::getOpenFileName(0, "Select model");
    else
        model = parser.value("model");
    
    if (directory.isEmpty() || directory.isNull() || model.isEmpty() || model.isNull()) return 0;
    
    PCA *pca = new PCA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, 0, 0);

    cv::FileStorage strainStorage(model.toStdString(), cv::FileStorage::READ);
    //cv::FileStorage strainStorage("potkani", cv::FileStorage::READ);
    strain->deserialize(strainStorage);

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    qDebug() << "Tracker initializated";

    // create GUI
    WindowAnotationManager w(directory, tracker);
    w.show();
    return app.exec();
}
