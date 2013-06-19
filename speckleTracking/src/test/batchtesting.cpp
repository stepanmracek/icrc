#include "batchtesting.h"

#include <QString>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>

#include "linalg/vecf.h"
#include "linalg/shapecomparer.h"
#include "linalg/common.h"
#include "linalg/frequencymodulation.h"
#include "linalg/serialization.h"
#include "strain/strain.h"
#include "strain/shapetracker.h"
#include "strain/shapenormalizer.h"
#include "strain/longitudinalstrain.h"
#include "strain/pointtrackeropticalflow.h"
#include "strain/pointtrackerneighbouropticalflow.h"
#include "strain/pointtrackerdistance.h"
#include "strain/strainstatistics.h"

void testTracker(ShapeTracker &tracker, const QString &dirPath)
{
    float b2bVariance = 0.0f;
    float b2bVariancePerSegment = 0.0f;
    float meanDiff = 0.0f;

    QDir dir(dirPath, "*.avi");
    QFileInfoList videos = dir.entryInfoList();
    foreach (const QFileInfo &info, videos)
    {
        VideoDataClip clip(info.absoluteFilePath(), info.absoluteFilePath()+"_metadata");

        // get beat indicies
        QVector<int> &beats = clip.getMetadata()->beatIndicies;
        ShapeMap &rawShapes = clip.getMetadata()->rawShapes;
        clip.getMetadata()->getCoordSystem()->resultMatRows = 300;
        clip.getMetadata()->getCoordSystem()->resultMatCols = 200;
        ShapeMap resultShapeMap;
        VectorF diffs;
        foreach(int beat, beats)
        {
            // check for raw control points
            if (beat >= clip.size()-1) continue;
            if (!rawShapes.contains(beat)) continue;

            // create shape
            Points &rawShape = rawShapes[beat];
            Points shape = tracker.getStrain()->getRealShapePoints(rawShape, 20);

            // track
            int beatStart, beatEnd;
            clip.getBeatRange(beat, beatStart, beatEnd);

            ShapeMap shapemap = tracker.track(&clip, beatStart, beatEnd, shape);

            QMapIterator<int, Points> iter(shapemap);
            while (iter.hasNext())
            {
                iter.next();
                resultShapeMap[iter.key()] = iter.value();
            }

            float d = ShapeComparer::optimalDiff(shapemap[beatStart], shapemap[beatEnd-1]);
            diffs.push_back(d);
        }

        // evaluate stats
        QVector<StrainStatistics> stats = StrainStatistics::getAllBeatsStats(&clip, tracker.getStrain(), resultShapeMap);
        b2bVariance += StrainStatistics::beatToBeatVariance(stats, 100);
        b2bVariancePerSegment += VecF::meanValue(StrainStatistics::beatToBeatVariancePerSegment(stats, 100));
        meanDiff += VecF::meanValue(diffs);
    }

    b2bVariance /= videos.count();
    b2bVariancePerSegment /= videos.count();
    meanDiff /= videos.count();
    qDebug() << b2bVariance << b2bVariancePerSegment << meanDiff;
}

PointTrackerBase * getPointTracker(int index)
{
    switch (index)
    {
    case 0:
        return new PointTrackerOpticalFlow(21);
    case 1:
        return new PointTrackerNeighbourOpticalFlow(20, 21, 3);
    case 2:
        return new PointTrackerDistance(new CosineMetric(), 21);
    }
}

void BatchTesting::process()
{
    QString videoDirectory = "/home/stepo/Dropbox/projekty/icrc/test2/";
    QString rawShapesPath =  "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
    VectorOfShapes rawShapes = Serialization::readVectorOfShapes(rawShapesPath);
    ListOfImageProcessing imgProc;

    QList<float> freqStart; freqStart << 1.0 << 1.0 << 1.0;
    QList<float> freqEnd; freqEnd     << 3.0 << 3.0 << 3.0;
    QList<float> freqStep; freqStep   << 1.0 << 1.0 << 1.0;
    QList<float> phase; phase         << 3.0 << 3.0 << 3.0;
    QList<float> amplitude; amplitude << 1.0 << 0.5 << 0.2;

    QList<int> segments; segments                 <<  6 << 6 << 6;
    QList<int> pointsPerSegment; pointsPerSegment << 10 << 5 << 3;

    QList<int> resProcWindowSize; resProcWindowSize << 1 << 3 << 5 << 7;

    QList<VectorF> weights;
    VectorF w1; w1.push_back(1.0);
    VectorF w2; w2.push_back(1); w2.push_back(0.5);
    VectorF w3; w3.push_back(1); w3.push_back(0.5); w3.push_back(0.25);
    weights << w1 << w2 << w3;

    int freqCount = freqStart.count();
    int segCount = segments.count();
    int resProcCount = resProcWindowSize.count();
    int weightsCount = weights.count();
    int pTrackerCount = 3;

    for (int freqIndex = 0; freqIndex < freqCount; freqIndex++)
    {
        for (int segIndex = 0; segIndex < segCount; segIndex++)
        {
            std::vector<VectorF> modValues =
                    FrequencyModulation::generateModulationValues(segments[segIndex]*pointsPerSegment[segIndex] + 1,
                                                                  freqStart[freqIndex], freqEnd[freqIndex], freqStep[freqIndex],
                                                                  phase[freqIndex], amplitude[freqIndex]);
            int modValuesCount = modValues.size();

            ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
            LongitudinalStrain dummyStrain(dummyNormalizer, segments[segIndex], pointsPerSegment[segIndex]);

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

            for (int resProcIndex = 0; resProcIndex < resProcCount; resProcIndex++)
            {
                for (int wIndex = 0; wIndex < weightsCount; wIndex++)
                {
                    for (int pTrackerIndex = 0; pTrackerIndex < pTrackerCount; pTrackerIndex++)
                    {
                        PointTrackerBase *pointTracker = getPointTracker(pTrackerIndex);

                        StrainResultProcessingBase *resProc = resProcWindowSize[resProcIndex] == 1 ?
                                    (StrainResultProcessingBase*)(new StrainResultProcessingPass()) :
                                    (StrainResultProcessingBase*)(new StrainResProcFloatingAvg(resProcWindowSize[resProcIndex]));

                        PCA *pca = new PCA();
                        StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
                        ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
                        LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments[segIndex], pointsPerSegment[segIndex]);
                        ShapeTracker tracker(strain, imgProc, pointTracker, resProc, weights[wIndex]);

                        qDebug() << tracker.getInfo().toStdString().c_str();
                        qDebug() << "Freq" << freqStart[freqIndex] << freqEnd[freqIndex] << freqStep[freqIndex] << phase[freqIndex] << amplitude[freqIndex];
                        testTracker(tracker, videoDirectory);
                    }
                }
            }
        }
    }
}
