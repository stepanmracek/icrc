#include "batchtesting.h"

#include <QString>
#include <QDebug>

#include "strain/strain.h"
#include "strain/shapetracker.h"
#include "linalg/common.h"
#include "linalg/frequencymodulation.h"
#include "linalg/serialization.h"
#include "strain/shapenormalizer.h"
#include "strain/longitudinalstrain.h"
#include "strain/pointtrackeropticalflow.h"

void BatchTesting::process()
{
    QString videoDirectory = "/home/stepo/ownCloud/icrc/test2/";
    QString rawShapesPath = "/home/stepo/ownCloud/icrc/dataDir/rawControlPoints";
    qDebug() << rawShapesPath;
    VectorOfShapes rawShapes = Serialization::readVectorOfShapes(rawShapesPath);
    ListOfImageProcessing imgProc;

    QList<float> freqStart; freqStart << 1.0;
    QList<float> freqEnd; freqEnd     << 3.0;
    QList<float> freqStep; freqStep   << 1.0;
    QList<float> phase; phase         << 3.0;
    QList<float> amplitude; amplitude << 1.0;

    QList<int> segments; segments                 << 6;
    QList<int> pointsPerSegment; pointsPerSegment << 6;

    QList<int> resProcWindowSize; resProcWindowSize << 1;

    QList<VectorF> weights;
    VectorF w1; w1.push_back(1.0);

    int n = freqStart.count();
    int m = segments.count();
    int o = resProcWindowSize.count();
    int p = weights.count();

    for (int i = 0; i < n; i++)
    {
        std::vector<VectorF> modValues =
                FrequencyModulation::generateModulationValues(segments[i]*pointsPerSegment[i] + 1,
                                                              freqStart[i], freqEnd[i], freqStep[i],
                                                              phase[i], amplitude[i]);
        int modValuesCount = modValues.size();

        for (int j = 0; j < m; j++)
        {
            ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
            LongitudinalStrain dummyStrain(dummyNormalizer, segments[i], pointsPerSegment[i]);

            VectorOfShapes shapes;
            foreach (const Points &controlPoints, rawShapes)
            {
                for (int width = 20; width <= 30; width += 10)
                {
                    for (int i = 0; i < modValuesCount; i++)
                    {
                        Points shape = dummyStrain.getRealShapePoints(controlPoints, width, &modValues[i]);
                        shapes.push_back(shape);
                    }
                }
            }

            for (int k = 0; k < o; k++)
            {

                for (int l = 0; l < p; l++)
                {
                    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);

                    StrainResultProcessingBase *resProc = resProcWindowSize[k] == 1 ?
                                (StrainResultProcessingBase*)(new StrainResultProcessingPass()) :
                                (StrainResultProcessingBase*)(new StrainResProcFloatingAvg(resProcWindowSize[k]));

                    PCA *pca = new PCA();
                    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
                    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
                    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments[j], pointsPerSegment[j]);
                    ShapeTracker tracker(strain, imgProc, pointTracker, resProc, weights[l]);
                }
            }
        }
    }
}
