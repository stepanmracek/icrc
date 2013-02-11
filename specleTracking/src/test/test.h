#ifndef TEST_H
#define TEST_H

#include <QApplication>

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
#include "ui/widgetstrainstatistics.h"
#include "strain/videodataclip.h"
#include "strain/pointtrackerneighbouropticalflow.h"
#include "strain/imagefilter.h"
#include "strain/strainresultprocessing.h"
#include "ui/widgetanotation.h"
#include "ui/uiutils.h"
#include "ui/widgetanotationmanager.h"
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

        VectorOfShapes after = Serialization::readShapeList("test-serialization-listOfPoints");
        Serialization::serialize(after, "test-serialization-listOfPoints2");
    }

    static void testAnotation()
    {
        VideoDataClip data("test.wmv");
        VectorOfShapes shapes = Anotation::anotateFromVideo(data);
        Serialization::serialize(shapes, "shapes");
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

    static void testLearnPCAShape()
    {
        PCA pca;
        QMap<int, Points> map = Serialization::readShapeMap("test/test_shapemap_26");
        VectorOfShapes shapes = Common::MapToVectorOfShapes(map);

        StatisticalShapeModel model(pca, shapes);
        StatisticalShapeModel::showStatisticalShape(pca);
        pca.serialize("test/pca-shape-radialBase");
    }

    static void testTracking()
    {
        VideoDataClip data("test.wmv");
        Mat8 firstFrame;
        data.getNextFrame(firstFrame);
        Points initialPoints = Anotation::anotateFromFrame(firstFrame);
        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);
        PointTrackerOpticalFlow tracker(20);
        PCA pca("pca-shape-radialBase");
        StatisticalShapeModel shapeModel(pca);
        ShapeNormalizerStatisticalShape normalizer(shapeModel);
        ListOfImageProcessing processing;
        StrainResultProcessingPass postProcessing;
        LongitudinalStrain strain(normalizer);

        //ShapeTracker::track(data, strain, processing, tracker, postProcessing, initialPoints, true);
    }

    static void testStatisticalShapeChanges()
    {
        PCA model;
        int framesCount = 10;
        VectorOfShapes shapes = Serialization::readShapeList("anotated-50");
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

    static void testAutomaticTracking()
    {
        VideoDataClip data("test.wmv");

        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);
        PCA pca("pca");
        StatisticalShapeModel shapeModel(pca);
        ShapeNormalizerIterativeStatisticalShape normalizer(shapeModel);
        PointTrackerOpticalFlow tracker(20);
        ListOfImageProcessing processing;
        StrainResultProcessingPass postProcessing;
        LongitudinalStrain strain(normalizer);
        Points input;

        //VectorOfShapes resultShapes = ShapeTracker::track(data, strain, processing, tracker, postProcessing, input);
        //Serialization::serialize(resultShapes, "trackingResult");
    }

    /*static int testStatistics()
    {
        VectorOfShapes resultShapes = Serialization::readShapeList("trackingResult");
        VideoDataClip video("test.wmv");
        StrainStatistics statistics = StrainStatistics(resultShapes);

        QApplication app(0, 0);
        WidgetStrainStatistics w;
        QMap<int, Points> map = Common::shapeListToMap(resultShapes);
        Strain strain()
        w.SetData(&statistics, );
        w.SetData(&statistics, &video, video. &resultShapes, map);
        w.show();

        return app.exec();
    }*/

    static void testOpticalFlowTracking()
    {
        VectorOfShapes shapes = Serialization::readShapeList("anotated-50");
        VideoDataClip data("test.wmv");
        Spline spline;

        VectorOfShapes uniformShapes;
        int n = shapes.size();
        for (int i = 0; i < n; i++)
        {
            Points uniformShape = spline.uniformDistance(shapes[i], 10, true);
            uniformShapes.push_back(uniformShape);
        }

        PCA pca;
        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);
        StatisticalShapeModel model(pca, uniformShapes);

        PointTrackerNeighbourOpticalFlow tracker;
        //ShapeNormalizerStatisticalShape normalizer(model);
        ShapeNormalizerIterConfStatShape normalizer(model);

        //ShapeNormalizerPass normalizer2(initialPoints.size());

        ListOfImageProcessing processing;
        /*ImageFilterMedian median;
        ImageFilterHistEq histEq;
        ImageFilterNlMeansDenoise nlMeans(10);
        processing.push_back(&median);
        processing.push_back(&histEq);
        processing.push_back(&nlMeans);*/

        StrainResProcFloatingAvg postProcessing(1);
        //StrainResultProcessingPass postProcessing;

        LongitudinalStrain strain(normalizer);

        //VectorOfShapes resultShapes = ShapeTracker::track(data, strain, processing, tracker, postProcessing, shapes[0], true);
        //Serialization::serialize(resultShapes, "trackingResult");
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

    static void testOpticalFlowIntensityMap()
    {
        VideoDataClip data("test.wmv");
        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);

        PointTrackerNeighbourOpticalFlow t(500);
        for (int i = 1; i < data.size(); i++)
        {
            Mat8 prev = data.frames[i-1];
            Mat8 next = data.frames[i];

            Mat8 transformedPrev = coord.transform(prev);
            Mat8 transformedNext = coord.transform(next);

            MatF intensity = t.trackIntensity(transformedPrev, transformedNext);

            float min, max;
            Common::getMinMax(intensity, min, max);
            intensity = (intensity-min)/(max - min);

            cv::imshow("intensity", intensity);
            cv::waitKey(1);
        }
    }

    static int testQtAnotation(int argc, char *argv[])
    {
        QApplication app(argc, argv);

        VideoDataClip data("test.wmv");
        QPixmap image = UIUtils::Mat8ToQPixmap(data.frames[0]);

        WidgetAnotation w;
        w.setImage(image);
        w.show();

        return app.exec();
    }

    static int testQtManager(int argc, char *argv[])
    {
        PCA pca("/home/stepo/SparkleShare/private/icrc/test/pca-shape");
        CoordSystemRadial coord(P(263,0), P(632,258), P(10,360), 50, 200, 400);
        StatisticalShapeModel model(pca);
        ShapeNormalizerIterativeStatisticalShape normalizer(model);
        ListOfImageProcessing processing;
        StrainResProcFloatingAvg postProcessing(1);
        PointTrackerOpticalFlow pointTracker(20);
        LongitudinalStrain ls(normalizer);
        float weightValues[] = {10.0, 5.0, 3.0, 2.0, 1.0};
        VectorF weights(weightValues, weightValues + sizeof(weightValues)/sizeof(float));
        ShapeTracker tracker(ls, &coord, processing, pointTracker, postProcessing, weights);

        // create GUI
        QApplication app(argc, argv);
        WidgetAnotationManager w("/home/stepo/SparkleShare/private/icrc/test/", &tracker);
        w.show();
        return app.exec();
    }

    static void testLongitudinalStrain()
    {
        ShapeNormalizerPass shapeNorm(30);
        LongitudinalStrain ls(shapeNorm);

        VideoDataClip data("test.wmv");
        Points basePoints = Anotation::anotateFromFrame(data.frames[0]);

        Points shapePoints = ls.getRealShapePoints(basePoints, 20);

        Anotation::showShape(data.frames[0], shapePoints);
    }

    static int testQtAnotationAndDisplay(int argc, char *argv[])
    {
        QApplication app(argc, argv);

        VideoDataClip data("test.wmv");
        QPixmap image = UIUtils::Mat8ToQPixmap(data.frames[0]);

        WidgetAnotation w;
        w.setImage(image);
        w.show();

        return app.exec();
    }
};

#endif // TEST_H
