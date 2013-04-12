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
#include "strain/statisticalshapechanges.h"
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
        VideoDataClip data("/home/stepo/ownCloud/icrc/test/test.wmv");
        VectorOfShapes shapes = Anotation::anotateFromVideo(data);
        Serialization::serialize(shapes, "/home/stepo/ownCloud/icrc/dataDir/rawShapes");
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
        QString pathToRawControlPoints = "/home/stepo/ownCloud/icrc/dataDir/rawControlPoints";
        VectorOfShapes rawShapes = Serialization::readVectorOfShapes(pathToRawControlPoints);

        ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
        LongitudinalStrain dummyStrain(dummyNormalizer);

        VectorOfShapes shapes;
        foreach (const Points &controlPoints, rawShapes)
        {
            for (int width = 30; width <= 40; width += 2)
            {
                Points shape = dummyStrain.getRealShapePoints(controlPoints, width);
                shapes.push_back(shape);
            }
        }

        PCA *pca = new PCA();
        StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
        pca->serialize("/home/stepo/ownCloud/icrc/dataDir/pca-shape2");

        /*ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
        LongitudinalStrain *strain = new LongitudinalStrain(normalizer);
        ListOfImageProcessing imgProc;
        PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(20);
        StrainResultProcessingBase *resProc = new StrainResultProcessingPass();
        VectorF weights; weights.push_back(1);
        ShapeTracker tracker(strain, imgProc, pointTracker, resProc, weights);

        QApplication app(0, 0);

        DialogShapeModel dlgShapeModel(pca, &tracker);
        dlgShapeModel.show();

        app.exec();*/
    }

    static void testStatisticalShapeChanges()
    {
        PCA model;
        int framesCount = 10;
        VectorOfShapes shapes = Serialization::readVectorOfShapes("anotated-50");
        StatisticalShapeChanges changes(shapes, framesCount, model);
        std::vector<Points> deltas = changes.createDeltas(shapes);
        model.modesSelectionThreshold();

        for (size_t i = 0; i < deltas.size()-framesCount; i++)
        {
            std::cout << "Iteration: " << i << std::endl;
            std::vector<Points> input;
            for (int j = 0; j < (framesCount-1); j++)
            {
                int realIndex = i+j;
                input.push_back(deltas[realIndex]);
            }

            Points desiredOutput = deltas[i + (framesCount-1)];
            Points output = changes.predict(input);

            MatF diff = (Common::pointsToMatF(desiredOutput) - Common::pointsToMatF(output)); //.t();
            float absDiff = Common::absSum(diff);
            std::cout << "  difference between original and predicted: " << absDiff << std::endl;
            //Common::printMatrix(diff);
        }
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
        QString dataDir = "/home/stepo/ownCloud/icrc/dataDir";
        PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape2");
        StatisticalShapeModel *model = new StatisticalShapeModel(pca);
        ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
        ListOfImageProcessing processing;
        StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
        PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(20); // new PointTrackerNeighbourOpticalFlow(20, 11, 2);
        Strain *ls = new LongitudinalStrain(normalizer);
        float weightValues[] = {1.0f, 0.5f};
        VectorF weights(weightValues, weightValues + sizeof(weightValues)/sizeof(float));
        ShapeTracker *tracker = new ShapeTracker(ls, processing, pointTracker, postProcessing, weights);

        qDebug() << "Tracker initializated";

        // create GUI
        QApplication app(argc, argv);
        WindowAnotationManager w("/home/stepo/ownCloud/icrc/test2/", dataDir, tracker);
        w.show();
        return app.exec();
    }

    static void testBeatToBeatVariance()
    {
        VideoDataClip clip("/home/stepo/SparkleShare/private/icrc/test/test.wmv",
                           "/home/stepo/SparkleShare/private/icrc/test/test.wmv_metadata");
        ShapeMap shapeMap = Serialization::readShapeMap("/home/stepo/SparkleShare/private/icrc/test/test.wmv_shapemap");

        PCA *pca = new PCA("/home/stepo/SparkleShare/private/icrc/test/pca-shape");
        StatisticalShapeModel *model = new StatisticalShapeModel(pca);
        ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
        LongitudinalStrain strain(normalizer);

        VideoDataClip firstClip; VectorOfShapes firstShapes; ShapeMap firstMap;
        clip.getSubClip(clip.getMetadata()->beatIndicies[0], shapeMap, &firstClip, firstShapes, firstMap);
        StrainStatistics firstStats(&strain, firstShapes);

        VideoDataClip secondClip; VectorOfShapes secondShapes; ShapeMap secondMap;
        clip.getSubClip(clip.getMetadata()->beatIndicies[1], shapeMap, &secondClip, secondShapes, secondMap);
        StrainStatistics secondStats(&strain, secondShapes);

        qDebug() << StrainStatistics::beatToBeatVariance(firstStats, secondStats, 100);
    }
};

#endif // TEST_H
