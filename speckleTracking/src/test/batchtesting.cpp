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

struct testTrackerResult
{
    float mainStrainVariance;
    float perSegmentStrainVariance;
    float ssd;
};

testTrackerResult testTracker(ShapeTracker &tracker, const QList<VideoDataClip*> videos)
{
    float b2bVariance = 0.0f;
    float b2bVariancePerSegment = 0.0f;
    float meanDiff = 0.0f;

    foreach (const VideoDataClip *clip, videos)
    {
        // get beat indicies
        QVector<int> &beats = clip->getMetadata()->beatIndicies;
        ShapeMap &rawShapes = clip->getMetadata()->rawShapes;
        //clip.getMetadata()->getCoordSystem()->resultMatRows = 300; ???
        //clip.getMetadata()->getCoordSystem()->resultMatCols = 200;
        ShapeMap resultShapeMap;
        VectorF diffs;
        foreach(int beat, beats)
        {
            // check for raw control points
            //if (beat >= clip.size()-1) continue;
            //if (!rawShapes.contains(beat)) continue;

            // create shape
            Points &rawShape = rawShapes[beat];
            Points shape = tracker.getStrain()->getRealShapePoints(rawShape, 20);

            // track
            int beatStart, beatEnd;
            clip->getBeatRange(beat, beatStart, beatEnd);
            if (beatStart == -1 || beatEnd == -1) continue;

            ShapeMap shapemap = tracker.track(clip, beatStart, beatEnd, shape);

            QMapIterator<int, Points> iter(shapemap);
            while (iter.hasNext())
            {
                iter.next();
                assert(!resultShapeMap.contains(iter.key()));
                resultShapeMap[iter.key()] = iter.value();
            }

            float d = ShapeComparer::optimalDiff(shapemap[beatStart], shapemap[beatEnd-1]);
            diffs.push_back(d);
        }

        // evaluate stats
        QVector<StrainStatistics> stats = StrainStatistics::getAllBeatsStats(clip, tracker.getStrain(), resultShapeMap);
        b2bVariance += StrainStatistics::beatToBeatVariance(stats, 100);
        b2bVariancePerSegment += VecF::meanValue(StrainStatistics::beatToBeatVariancePerSegment(stats, 100));
        meanDiff += VecF::meanValue(diffs);
    }

    b2bVariance /= videos.count();
    b2bVariancePerSegment /= videos.count();
    meanDiff /= videos.count();

    testTrackerResult result;
    result.mainStrainVariance = b2bVariance;
    result.perSegmentStrainVariance = b2bVariancePerSegment;
    result.ssd = meanDiff;
    return result;
}

PointTrackerBase * getPointTracker(int index)
{
    switch (index)
    {
    case 0:
        return new PointTrackerOpticalFlow(21);
    case 1:
        return new PointTrackerNeighbourOpticalFlow(21, 21, 3);
    case 2:
        return new PointTrackerDistance(new CosineMetric(), 21);
    }
    return 0;
}

QString getTrackerInfo(int index)
{
    switch (index)
    {
    case 0:
        return "optical-21";
    case 1:
        return "optical-neighbor-21-21-3";
    case 2:
        return "cosine-21";
    }
    return "";
}

void BatchTesting::process()
{
    QString videoDirectoryPath = "/home/stepo/Dropbox/projekty/icrc/test3/";
    QString rawShapesPath =  "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
    VectorOfShapes rawShapes = Serialization::readVectorOfShapes(rawShapesPath);
    ListOfImageProcessing imgProc;

    QList<VideoDataClip*> videos;

    QDir videoDirPath(videoDirectoryPath, "*.mp4");
    QFileInfoList videoFiles = videoDirPath.entryInfoList();
    foreach (const QFileInfo &info, videoFiles)
    {
        videos << new VideoDataClip(info.absoluteFilePath(), info.absoluteFilePath()+"_metadata");
        qDebug() << "loaded" << info.baseName() << "with" << (videos.last()->getMetadata()->beatIndicies.count()-1) << "beats";
    }

    float freqStart = 1.0;
    float freqEnd = 3.0;
    float freqStep = 1.0;
    float phaseSteps = 3.0;
    //QList<float> freqStart; freqStart << 1.0 << 1.0 << 1.0;
    //QList<float> freqEnd; freqEnd     << 3.0 << 3.0 << 3.0;
    //QList<float> freqStep; freqStep   << 1.0 << 1.0 << 1.0;
    //QList<float> phase; phase         << 3.0 << 3.0 << 3.0;
    QList<float> amplitude; amplitude << 0.2; //<< 1.0 << 0.5 << 0.2;

    //QList<int> segments; segments                 <<  6 << 6 << 6;
    int segments = 6;
    QList<int> pointsPerSegment; pointsPerSegment << 10 << 5 << 3;

    QList<int> resProcWindowSize; resProcWindowSize << 1 << 3 << 5 << 7;

    QList<VectorF> weights;
    VectorF w1; w1.push_back(1.0);
    VectorF w2; w2.push_back(1); w2.push_back(0.5);
    VectorF w3; w3.push_back(1); w3.push_back(0.5); w3.push_back(0.25);
    weights << w1 << w2 << w3;

    int resProcCount = resProcWindowSize.count();
    int weightsCount = weights.count();
    int pTrackerCount = 3;

    qDebug() << "amplitude pointsPerSegment resultProcWindowSize weightsCount pTracker strainVariance perSegmentVariance diff";
    for (int amplitudeIndex = 0; amplitudeIndex < amplitude.count(); amplitudeIndex++)
    {
        for (int segIndex = 0; segIndex < pointsPerSegment.count(); segIndex++)
        {
            std::vector<VectorF> modValues =
                    FrequencyModulation::generateModulationValues(segments*pointsPerSegment[segIndex] + 1,
                                                                  freqStart, freqEnd, freqStep, phaseSteps, amplitude[amplitudeIndex]);
            int modValuesCount = modValues.size();

            ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
            LongitudinalStrain dummyStrain(dummyNormalizer, segments, pointsPerSegment[segIndex]);

            VectorOfShapes shapes;
            foreach (const Points &controlPoints, rawShapes)
            {
                for (int width = 20; width <= 30; width += 10)
                {
                    for (int i = 0; i < modValuesCount; i++)
                    {
                        Points shape = dummyStrain.getRealShapePoints(controlPoints, width, &modValues[i], 0);
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
                        LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments, pointsPerSegment[segIndex]);
                        ShapeTracker tracker(strain, imgProc, pointTracker, resProc, weights[wIndex]);

                        testTrackerResult result = testTracker(tracker, videos);
                        qDebug() << amplitude[amplitudeIndex] << pointsPerSegment[segIndex] << resProcWindowSize[resProcIndex]
                                    << weights[wIndex].size() << getTrackerInfo(pTrackerIndex)
                                    << result.mainStrainVariance << result.perSegmentStrainVariance << result.ssd;
                    }
                }
            }
        }
    }
}
