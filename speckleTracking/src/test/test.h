#ifndef TEST_H
#define TEST_H

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <list>
#include <vector>
#include <iostream>

#include "linalg/common.h"
#include "linalg/serialization.h"
#include "strain/anotation.h"
#include "linalg/pca.h"
#include "strain/statisticalshapemodel.h"
#include "strain/coordsystem.h"
#include "strain/shapetracker.h"
#include "strain/pointtrackeropticalflow.h"
#include "strain/shapenormalizer.h"
#include "strain/strainstatistics.h"
#include "ui/dialogstrainstatistics.h"
#include "strain/videodataclip.h"
#include "strain/pointtrackerneighbouropticalflow.h"
#include "strain/imagefilter.h"
#include "strain/strainresultprocessing.h"
#include "ui/dialogshapemodel.h"
#include "ui/widgetanotation.h"
#include "ui/uiutils.h"
#include "ui/windowanotationmanager.h"
#include "strain/longitudinalstrain.h"
#include "linalg/frequencymodulation.h"
#include "linalg/widthmodulation.h"

class Test
{
public:
    static void testSerialization()
    {
        VectorOfShapes before;

        Points before0;
        before0.push_back(P(1,2));
        before0.push_back(P(3,4));

        Points before1;
        before1.push_back(P(5,6));
        before1.push_back(P(7,8));

        before.push_back(before0);
        before.push_back(before1);

        Serialization::serialize(before, "test-serialization-listOfPoints");

        VectorOfShapes after = Serialization::readVectorOfShapes("test-serialization-listOfPoints");
        Serialization::serialize(after, "test-serialization-listOfPoints2");
    }

    static void testAnotation()
    {
        VideoDataClip data("/home/stepo/Dropbox/projekty/icrc/test/test.wmv");
        VectorOfShapes shapes = Anotation::anotateFromVideo(data);
        Serialization::serialize(shapes, "/home/stepo/Dropbox/projekty/icrc/dataDir/rawShapes");
    }

    static void testUniformSpline()
    {
        VideoDataClip data("test.wmv");
        Points shape = Anotation::anotateFromFrame(data.frames[0]);
        Spline spline;
        Points uniformShape = spline.uniformDistance(shape, 15, true);
        assert(uniformShape.size() == 15);
        Anotation::showShape(data.frames[0], uniformShape);

    }

    static void testLearnShape()
    {
        QString pathToRawControlPoints = "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
        VectorOfShapes rawShapes = Serialization::readVectorOfShapes(pathToRawControlPoints);

        ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
        LongitudinalStrain dummyStrain(dummyNormalizer, 6, 5);

        VectorOfShapes shapes;
        foreach (const Points &controlPoints, rawShapes)
        {
            for (int width = 10; width <= 40; width += 5)
            {
                Points shape = dummyStrain.getRealShapePoints(controlPoints, width);
                shapes.push_back(shape);
            }
        }

        PCA *pca = new PCA();
        StatisticalShapeModel(pca, shapes);
        pca->serialize("/home/stepo/Dropbox/projekty/icrc/dataDir/pca-shape2");
    }

    static void testFreqModulation()
    {
        /*QString pathToRawControlPoints = "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
        Points rawControlPoints = Serialization::readVectorOfShapes(pathToRawControlPoints)[0];
        Spline spline;
        Points uniformControlPoints = spline.uniformDistance(rawControlPoints, 6, false);
        Points uniformPoints = spline.getSplinePoints(uniformControlPoints, 5);*/

        FrequencyModulation::test();
    }

    static void testWidthModulation()
    {
        WidthModulation::test();
    }

    static void testLearnShapeWithFM()
    {
        int segmentsCount = 6;
        int pointsPerSegment = 10;

        std::vector<VectorF> modValues =
                FrequencyModulation::generateModulationValues(segmentsCount*pointsPerSegment + 1 , 1.0, 3.0, 1.0, 3.0, 0.2);
        int modValuesCount = modValues.size();

        QString pathToRawControlPoints = "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
        VectorOfShapes rawShapes = Serialization::readVectorOfShapes(pathToRawControlPoints);

        ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
        LongitudinalStrain dummyStrain(dummyNormalizer, segmentsCount, pointsPerSegment);

        VectorOfShapes shapes;
        qDebug() << "raw shapes" << rawShapes.size();
        foreach (const Points &controlPoints, rawShapes)
        {
            for (int width = 20; width <= 30; width += 10)
            {
                for (int i = 0; i < modValuesCount; i++)
                {
                    Points shape = dummyStrain.getRealShapePoints(controlPoints, width, &modValues[i], 0);
                    shapes.push_back(shape);
                }
                //break; // TODO remove later
            }
            //break; // TODO remove later
        }
        qDebug() << "processed shapes" << shapes.size();

        PCA *pca = new PCA();
        StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
        pca->setModes(20);
        qDebug() << "PCA modes" << pca->getModes() << pca->getMean().rows;

        ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
        LongitudinalStrain resultStrain(normalizer, segmentsCount, pointsPerSegment);
        resultStrain.serialize("/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10");

        // --------------------------------------------------------

        PCA *newPCA = new PCA();
        StatisticalShapeModel *newShapeModel = new StatisticalShapeModel(newPCA);
        ShapeNormalizerBase *newNormalizer = new ShapeNormalizerIterativeStatisticalShape(newShapeModel);
        LongitudinalStrain newStrain(newNormalizer, 0, 0);
        newStrain.deserialize("/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10");

        qDebug() << "newStrain:";
        qDebug() << "segmentsCount" << newStrain.segmentsCount;
        qDebug() << "pointsPerSegment" << newStrain.pointsPerSegment;
        qDebug() << "PCA modes meanSize" << newPCA->getModes() << newPCA->getMean().rows;
    }

    static void testImageProcessing()
    {
        cv::namedWindow("result");
        VideoDataClip data("test.wmv");
        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);
        for (int i = 0; i < data.size(); i++)
        {
            Mat8 transformed;
            data.getFrame(transformed, i);
            transformed = coord.transform(transformed);

            std::cout << "processing..." << std::endl;

            //cv::imshow("result", transformed);
            //cv::waitKey(500);

            ImageFilterNlMeansDenoise nlMeans(5,7,7);
            nlMeans.process(transformed);
            ImageFilterContrast contrast(2, 0);
            contrast.process(transformed);
            /*ImageFilterMedian median(11);
            median.process(transformed);
            ImageFilterEdge edge;
            edge.process(transformed);*/

            std::cout << "..done" << std::endl;

            cv::imshow("result", transformed);
            cv::waitKey(1);//1000);
        }
    }

    static int testQtManager(int argc, char *argv[])
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

        std::vector<VectorF> widthValues =
                WidthModulation::generateValues(pointsPerSegment*segments + 1, 1, 5, 1,
                                                10, 25.0f, 0.15f);
        int widthValuesCount = widthValues.size();

        ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
        LongitudinalStrain dummyStrain(dummyNormalizer, segments, pointsPerSegment);

        VectorOfShapes shapes;
        foreach (const Points &controlPoints, rawShapes)
        {
            for (int j = 0; j < widthValuesCount; j++) //(int width = 20; width <= 30; width += 10)
            {
                for (int i = 0; i < modValuesCount; i++)
                {
                    Points shape = dummyStrain.getRealShapePoints(controlPoints, 20, &modValues[i], &widthValues[j]);
                    shapes.push_back(shape);
                }
            }
            //break;
        }
        qDebug() << shapes.size();

        PCA *pca = new PCA();
        StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
        ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
        LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments, pointsPerSegment);

        //strain->deserialize("/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10");

        ListOfImageProcessing processing;
        StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(7);
        PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
        VectorF weights; weights.push_back(1.0f);
        ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

        qDebug() << "Tracker initializated";

        // create GUI
        QString dataDir = "/home/stepo/Dropbox/projekty/icrc/dataDir";
        QApplication app(argc, argv);
        WindowAnotationManager w("/home/stepo/Dropbox/projekty/icrc/test3/", dataDir, tracker);
        w.show();
        return app.exec();
    }

    static void testBeatToBeatVariance()
    {
        VideoDataClip clip("/home/stepo/Dropbox/projekty/icrc/test/test.wmv",
                           "/home/stepo/Dropbox/projekty/icrc/test/test.wmv_metadata");
        ShapeMap shapeMap = Serialization::readShapeMap("/home/stepo/SparkleShare/private/icrc/test/test.wmv_shapemap");

        PCA *pca = new PCA("/home/stepo/Dropbox/projekty/icrc/test/pca-shape");
        StatisticalShapeModel *model = new StatisticalShapeModel(pca);
        ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
        LongitudinalStrain strain(normalizer, 6, 5);

        VideoDataClip firstClip; VectorOfShapes firstShapes; ShapeMap firstMap;
        clip.getSubClip(clip.getMetadata()->beatIndicies[0], shapeMap, &firstClip, firstShapes, firstMap);
        StrainStatistics firstStats(&strain, firstShapes);

        VideoDataClip secondClip; VectorOfShapes secondShapes; ShapeMap secondMap;
        clip.getSubClip(clip.getMetadata()->beatIndicies[1], shapeMap, &secondClip, secondShapes, secondMap);
        StrainStatistics secondStats(&strain, secondShapes);

        qDebug() << StrainStatistics::beatToBeatVariance(firstStats, secondStats, 100);
    }

    static void testBeatRange()
    {
        QString dir("/home/stepo/Dropbox/projekty/icrc/test3/");
        VideoDataClip clip(dir + "vut_zd_adr_la_2.mp4", dir + "vut_zd_adr_la_2.mp4_metadata");

        QVector<int> testIndicies = clip.getMetadata()->beatIndicies;
        testIndicies << 0 << 2 << 52 << 499 << 500;
        int start, end;
        foreach (int beatIndex, testIndicies)
        {
            clip.getBeatRange(beatIndex, start, end);
            qDebug() << beatIndex << start << end;

            QMap<int, Points> dummyShapes;
            VideoDataClip outSubClip;
            VectorOfShapes outVecOfShapes;
            QMap<int, Points> outMapOfShapes;
            clip.getSubClip(beatIndex, dummyShapes, &outSubClip, outVecOfShapes, outMapOfShapes);
        }
    }
};

#endif // TEST_H
