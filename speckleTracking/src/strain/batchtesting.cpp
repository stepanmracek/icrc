#include "batchtesting.h"

#include <QDir>
#include <QDebug>

#include "pointtrackerneighbouropticalflow.h"
#include "pointtrackeropticalflow.h"
#include "shapetracker.h"
#include "longitudinalstrain.h"
#include "shapenormalizer.h"
#include "statisticalshapemodel.h"
#include "linalg/pca.h"
#include "videodataclip.h"
#include "linalg/serialization.h"
#include "strainstatistics.h"
#include "linalg/vecf.h"

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

void processOneTracker(ShapeTracker *tracker, QList<VideoDataClip *> &clips, QList<ShapeMap> &referenceShapeMaps, const QString &trackerName)
{
    qDebug() << "Processing" << trackerName;

    float cumulativeStdDev = 0;
    foreach (VideoDataClip *clip, clips)
    {
        int clipIndex = clips.indexOf(clip);
        ShapeMap resultShapes;

        //qDebug() << "  clip" << clipIndex;
        foreach (int beatIndex, clip->getMetadata()->beatIndicies)
        {
            //qDebug() << "    beat index" << beatIndex;
            int start,end;
            clip->getBeatRange(beatIndex, start, end);

            int frames = end - start - 1;

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
        }

        // generate statistics
        QVector<StrainStatistics> statsForClip = StrainStatistics::getAllBeatsStats(clip, tracker->getStrain(), resultShapes);
        int n = statsForClip.count();
        VectorF resampledBeats[n];
        for (int i = 0; i < n; i++)
        {
            const StrainStatistics &beat = statsForClip.at(i);
            resampledBeats[i] = VecF::resample(beat.strain, 100);
        }

        for (int i = 0; i < 100; i++)
        {
            VectorF slice;
            for (int j = 0; j < n; j++)
            {
                slice.push_back(resampledBeats[j][i]);
            }

            cumulativeStdDev += VecF::stdDeviation(slice);
        }
    }

    qDebug() << "Cumulative Std. dev.:" << cumulativeStdDev;
}

void BatchTesting::process()
{
    // create trackers
    ShapeTracker *flowWeights1 = getFlowWeights1();
    ShapeTracker *flow20_31_5_Weights1 = getFlow20_31_5_Weights1();
    ShapeTracker *flow20_11_2_Weights1 = getFlow20_11_2_Weights1();
    ShapeTracker *flowWeights2 = getFlowWeights2();
    ShapeTracker *flow20_31_5_Weights2 = getFlow20_31_5_Weights2();
    ShapeTracker *flow20_11_2_Weights2 = getFlow20_11_2_Weights2();

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

    processOneTracker(flowWeights1, clips, referenceShapeMaps, "flowWeights1");
    processOneTracker(flow20_31_5_Weights1, clips, referenceShapeMaps, "flow20_31_5_Weights1");
    processOneTracker(flow20_11_2_Weights1, clips, referenceShapeMaps, "flow20_11_2_Weights1");
    processOneTracker(flowWeights2, clips, referenceShapeMaps, "flowWeights2");
    processOneTracker(flow20_31_5_Weights2, clips, referenceShapeMaps, "flow20_31_5_Weights2");
    processOneTracker(flow20_11_2_Weights2, clips, referenceShapeMaps, "flow20_11_2_Weights2");
}
