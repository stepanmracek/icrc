#include "batchstraintracker.h"

#include "linalg/serialization.h"
#include "linalg/frequencymodulation.h"

void BatchStrainTracker::process()
{
    // params
    QString strainPath = "/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10";

    // initilize model
    PCA *pca = new PCA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, 0, 0);

    strain->deserialize(strainPath);

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    // load directory

    // for each video

    // track

    // serialize
}

ShapeTracker * BatchStrainTracker::learn()
{
    QString rawShapesPath =  "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
    VectorOfShapes rawShapes = Serialization::readVectorOfShapes(rawShapesPath);

    float freqStart = 1.0;
    float freqEnd = 3.0;
    float freqStep = 1.0;
    float phaseSteps = 3.0;
    double amplitude = 1.0;
    int pointsPerSegment = 10;
    int segments = 6;

    std::vector<VectorF> modValues =
            FrequencyModulation::generateModulationValues(segments*pointsPerSegment + 1,
                                                          freqStart, freqEnd, freqStep, phaseSteps, amplitude);
    int modValuesCount = modValues.size();

    //std::vector<VectorF> widthValues =
    //        WidthModulation::generateValues(pointsPerSegment*segments + 1, 1, 5, 1,
    //                                        10, 25.0f, 0.15f);
    //int widthValuesCount = widthValues.size();

    ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
    LongitudinalStrain dummyStrain(dummyNormalizer, segments, pointsPerSegment);

    VectorOfShapes shapes;
    foreach (const Points &controlPoints, rawShapes)
    {
        //for (int j = 0; j < widthValuesCount; j++)
        for (int width = 20; width <= 30; width += 10)
        {
            for (int i = 0; i < modValuesCount; i++)
            {
                Points shape = dummyStrain.getRealShapePoints(controlPoints, width, &modValues[i], 0); //, &widthValues[j]);
                shapes.push_back(shape);
            }
        }
    }

    PCA *pca = new PCA();
    //ICA *ica = new ICA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments, pointsPerSegment);

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    return tracker;
}
