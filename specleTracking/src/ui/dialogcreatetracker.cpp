#include "dialogcreatetracker.h"
#include "ui_dialogcreatetracker.h"

#include <QDebug>

#include "strain/shapetracker.h"
#include "strain/longitudinalstrain.h"
#include "strain/pointtrackerdistance.h"
#include "strain/pointtrackeropticalflow.h"

DialogCreateTracker::DialogCreateTracker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateTracker)
{
    ui->setupUi(this);

    modelNone = "None";
    modelPCANonIter = "PCA non-iterative";
    modelPCAIter = "PCA iterative";
    ui->cmbModel->addItem(modelNone);
    ui->cmbModel->addItem(modelPCANonIter);
    ui->cmbModel->addItem(modelPCAIter);

    pointTrackerSSD = "SSD";
    pointTrackerCorr = "Correlation";
    pointTrackerOptical = "Optical flow";
    ui->cmbPointTracker->addItem(pointTrackerSSD);
    ui->cmbPointTracker->addItem(pointTrackerCorr);
    ui->cmbPointTracker->addItem(pointTrackerOptical);

    weights1 = "(1)";
    weights2 = "(1, 0.5)";
    weights3 = "(1, 0.5, 0.25)";
    weights4 = "(1, 0.5, 0.25, 0.125)";
    ui->cmbWeights->addItem(weights1);
    ui->cmbWeights->addItem(weights2);
    ui->cmbWeights->addItem(weights3);
    ui->cmbWeights->addItem(weights4);
}

DialogCreateTracker::~DialogCreateTracker()
{
    delete ui;
}

void DialogCreateTracker::onCreateNewTrackerRequest()
{
    // model
    QString selectedModel = ui->cmbModel->currentText();
    ShapeNormalizerBase *normalizer;
    if (selectedModel.compare(modelNone) == 0)
    {
        qDebug() << "Selected model: None";
        normalizer = new ShapeNormalizerPass;
    }
    else if (selectedModel.compare(modelPCANonIter) == 0)
    {
        qDebug() << "Selected model: PCA non-iterative";
        PCA *pca = new PCA("/home/stepo/SparkleShare/private/icrc/test/pca-shape");
        StatisticalShapeModel *model = new StatisticalShapeModel(pca);
        normalizer = new ShapeNormalizerStatisticalShape(model);
    }
    else
    {
        qDebug() << "Selected model: PCA iterative";
        PCA *pca = new PCA("/home/stepo/SparkleShare/private/icrc/test/pca-shape");
        StatisticalShapeModel *model = new StatisticalShapeModel(pca);
        normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    }
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);

    // Point tracker
    QString selectedTracker = ui->cmbPointTracker->currentText();
    PointTrackerBase *pointTracker;
    if (selectedTracker.compare(pointTrackerSSD) == 0)
    {
        SumOfSquareDifferences *ssd = new SumOfSquareDifferences();
        pointTracker = new PointTrackerDistance(ssd, 10);
    }
    else if (selectedTracker.compare(pointTrackerCorr) == 0)
    {
        CorrelationMetric *corr = new CorrelationMetric();
        pointTracker = new PointTrackerDistance(corr, 10);
    }
    else
    {
        pointTracker = new  PointTrackerOpticalFlow(20);
    }

    // Weights
    VectorF weights;
    QString selectedWeights = ui->cmbWeights->currentText();
    if (selectedWeights.compare(weights1) == 0)
    {
        weights.push_back(1);
    }
    else if (selectedWeights.compare(weights2) == 0)
    {
        weights.push_back(1);
        weights.push_back(0.5);
    }
    else if (selectedWeights.compare(weights3) == 0)
    {
        weights.push_back(1);
        weights.push_back(0.5);
        weights.push_back(0.25);
    }
    else
    {
        weights.push_back(1);
        weights.push_back(0.5);
        weights.push_back(0.25);
        weights.push_back(0.125);
    }

    ListOfImageProcessing frameProcessing;
    StrainResultProcessingPass *resultProcessing = new StrainResultProcessingPass();
    newShapeTracker = new ShapeTracker(strain, frameProcessing, pointTracker, resultProcessing, weights);
    accept();
}
