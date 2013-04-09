#include "batchtesting.h"

#include <QDir>
#include <QDebug>
#include <QVector>

#include "pointtrackerneighbouropticalflow.h"
#include "pointtrackeropticalflow.h"
#include "pointtrackerdistance.h"
#include "shapetracker.h"
#include "longitudinalstrain.h"
#include "shapenormalizer.h"
#include "statisticalshapemodel.h"
#include "linalg/pca.h"
#include "videodataclip.h"
#include "linalg/serialization.h"
#include "strainstatistics.h"
#include "linalg/vecf.h"

QVector<ShapeNormalizerBase *> getNormalizers()
{
    QVector<ShapeNormalizerBase *> result;
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);

    result << (new ShapeNormalizerIterativeStatisticalShape(model));
    result << (new ShapeNormalizerPass());
    return result;
}

QVector<PointTrackerBase *> getPointTrackers()
{
    QVector<PointTrackerBase *> result;

    result << (new PointTrackerDistance(new SumOfSquareDifferences(), 11));
    result << (new PointTrackerDistance(new CorrelationMetric(), 11));
    result << (new PointTrackerDistance(new CosineMetric(), 11));
    result << (new PointTrackerDistance(new CityblockMetric(), 11));
    result << (new PointTrackerDistance(new SumOfSquareDifferences(), 21));
    result << (new PointTrackerDistance(new CorrelationMetric(), 21));
    result << (new PointTrackerDistance(new CosineMetric(), 21));
    result << (new PointTrackerDistance(new CityblockMetric(), 21));
    result << (new PointTrackerOpticalFlow(20));
    result << (new PointTrackerNeighbourOpticalFlow(20, 31, 5));
    result << (new PointTrackerNeighbourOpticalFlow(20, 11, 2));

    return result;
}

QVector<StrainResultProcessingBase *> getResultProcessings()
{
    QVector<StrainResultProcessingBase *> result;

    result << (new StrainResultProcessingPass());
    result << (new StrainResProcFloatingAvg(3));
    result << (new StrainResProcFloatingAvg(5));

    return result;
}

QVector<VectorF> getWeights()
{
    QVector<VectorF> result;

    VectorF weights1; weights1.push_back(1);
    VectorF weights2; weights2.push_back(1); weights2.push_back(0.5);
    VectorF weights3; weights3.push_back(1); weights3.push_back(0.5); weights3.push_back(0.25);

    result << weights1 << weights2 << weights3;
    return result;
}

/*
ShapeTracker *getFlowWeights1()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerOpticalFlow *pointTracker = new PointTrackerOpticalFlow(20);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}

ShapeTracker *getFlow20_31_5_Weights1()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerNeighbourOpticalFlow *pointTracker = new PointTrackerNeighbourOpticalFlow(20, 31, 5);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}

ShapeTracker *getFlow20_11_2_Weights1()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerNeighbourOpticalFlow *pointTracker = new PointTrackerNeighbourOpticalFlow(20, 11, 2);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}

ShapeTracker *getFlowWeights2()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerOpticalFlow *pointTracker = new PointTrackerOpticalFlow(20);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1); weights.push_back(0.5);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}

ShapeTracker *getFlow20_31_5_Weights2()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerNeighbourOpticalFlow *pointTracker = new PointTrackerNeighbourOpticalFlow(20, 31, 5);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1); weights.push_back(0.5);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}

ShapeTracker *getFlow20_11_2_Weights2()
{
    QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
    ListOfImageProcessing processing;
    PointTrackerNeighbourOpticalFlow *pointTracker = new PointTrackerNeighbourOpticalFlow(20, 11, 2);
    StrainResultProcessingPass *postProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1); weights.push_back(0.5);
    return new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);
}
*/

float diff(Points &first, Points &second)
{
    int n = first.size();
    assert(second.size() == n);

    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += Common::eucl(first[i], second[i]);
    }
    return sum/n;
}

void processOneTracker(ShapeTracker *tracker, QList<VideoDataClip *> &clips, QList<ShapeMap> &referenceShapeMaps)
{
    qDebug() << "Processing" << tracker->getInfo();

    float cumulativeStdDev = 0;
    float cumulativePerSegmentStdDev = 0;
    float cumulativeTargetDiff = 0;
    int beatCounter = 0;
    foreach (VideoDataClip *clip, clips)
    {
        int clipIndex = clips.indexOf(clip);
        ShapeMap resultShapes;

        //qDebug() << "  clip" << clipIndex;
        foreach (int beatIndex, clip->getMetadata()->beatIndicies)
        {
            beatCounter++;
            int start,end;
            clip->getBeatRange(beatIndex, start, end);
            int frames = end - start - 1;

            //qDebug() << "    beat index" << beatIndex << start << end << (start+frames);

            resultShapes[start] = referenceShapeMaps[clipIndex][beatIndex];
            for (int i = 0; i < frames; i++)
            {
                // prepare previous shapes and previous frames
                VectorOfShapes prevShapes;
                VectorOfImages prevFrames;

                for (unsigned int j = 0; j < tracker->weights.size(); j++)
                {
                    int prevIndex = beatIndex + i-j;

                    if (prevIndex < start)
                    {
                        break;
                    }

                    prevShapes.push_back(resultShapes[prevIndex]);
                    prevFrames.push_back(clip->frames[prevIndex]);
                }

                int nextIndex = beatIndex + i+1;
                Mat8 nextFrame = clip->frames[nextIndex];

                std::reverse(prevShapes.begin(), prevShapes.end());
                std::reverse(prevFrames.begin(), prevFrames.end());

                // track
                Points nextShape = tracker->track(prevFrames, prevShapes, nextFrame, clip->getMetadata()->getCoordSystem());
                resultShapes[nextIndex] = nextShape;
            }

            cumulativeTargetDiff += diff(resultShapes[start+frames], referenceShapeMaps[clipIndex][start+frames]);
        }

        // generate statistics
        QVector<StrainStatistics> statsForClip = StrainStatistics::getAllBeatsStats(clip, tracker->getStrain(), resultShapes);
        int n = statsForClip.count();
        int segmentsCount = tracker->getStrain()->segmentsCount;
        VectorF resampledBeats[n];
        VectorF resampledSegments[n][segmentsCount];
        for (int beatIndex = 0; beatIndex < n; beatIndex++)
        {
            const StrainStatistics &beat = statsForClip.at(beatIndex);
            resampledBeats[beatIndex] = VecF::resample(beat.strain, 100);

            for (int segmentIndex = 0; segmentIndex < segmentsCount; segmentIndex++)
            {
                resampledSegments[beatIndex][segmentIndex] = VecF::resample(beat.strainForSegments[segmentIndex], 100);
            }
        }

        for (int sampleIndex = 0; sampleIndex < 100; sampleIndex++)
        {
            VectorF slice;
            VectorF sliceSegments;
            for (int beatIndex = 0; beatIndex < n; beatIndex++)
            {
                slice.push_back(resampledBeats[beatIndex][sampleIndex]);

                for (int segmentIndex = 0; segmentIndex < segmentsCount; segmentIndex++)
                {
                    sliceSegments.push_back(resampledSegments[beatIndex][segmentIndex][sampleIndex]);
                }
            }

            cumulativeStdDev += VecF::stdDeviation(slice);
            cumulativePerSegmentStdDev += VecF::stdDeviation(sliceSegments);
        }
    }

    qDebug() << "  std. dev.:" << cumulativeStdDev << cumulativePerSegmentStdDev << (cumulativeStdDev+cumulativePerSegmentStdDev);
    qDebug() << "  target diff" << cumulativeTargetDiff/beatCounter;
}

void BatchTesting::process()
{
    // load data and reference shapes
    QString clip1Path = "/media/data/ownCloud/icrc/test/test.wmv";
    VideoDataClip *clip1 = new VideoDataClip(clip1Path, clip1Path+"_metadata");
    ShapeMap shapeMap1 = Serialization::readShapeMap(clip1Path+"_shapemap");
    QString clip2Path = "/media/data/ownCloud/icrc/test/z9BUATT02.wmv";
    VideoDataClip *clip2 = new VideoDataClip(clip2Path, clip2Path+"_metadata");
    ShapeMap shapeMap2 = Serialization::readShapeMap(clip2Path+"_shapemap");
    QString clip3Path = "/media/data/ownCloud/icrc/test/zA1BB7B02.wmv";
    VideoDataClip *clip3 = new VideoDataClip(clip3Path, clip3Path+"_metadata");
    ShapeMap shapeMap3 = Serialization::readShapeMap(clip3Path+"_shapemap");
    QString clip4Path = "/media/data/ownCloud/icrc/test2/C51BMQ80.avi";
    VideoDataClip *clip4 = new VideoDataClip(clip4Path, clip4Path+"_metadata");
    ShapeMap shapeMap4 = Serialization::readShapeMap(clip4Path+"_shapemap");

    QList<VideoDataClip*> clips;
    clips << clip1 << clip2 << clip3 << clip4;
    QList<ShapeMap> referenceShapeMaps;
    referenceShapeMaps << shapeMap1 << shapeMap2 << shapeMap3 << shapeMap4;

    // create trackers
    QVector<ShapeNormalizerBase*> normalizers = getNormalizers();
    QVector<PointTrackerBase*> pointTrackers = getPointTrackers();
    QVector<StrainResultProcessingBase*> resultProcessings = getResultProcessings();
    QVector<VectorF> weights = getWeights();


    ListOfImageProcessing processing;
    foreach(ShapeNormalizerBase *normalizer, normalizers)
    {
        foreach(PointTrackerBase *pointTracker, pointTrackers)
        {
            foreach(StrainResultProcessingBase *resultProcessing, resultProcessings)
            {
                foreach (VectorF w, weights)
                {
                    LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
                    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, resultProcessing, w);

                    processOneTracker(tracker, clips, referenceShapeMaps);
                }
            }
        }
    }
}
