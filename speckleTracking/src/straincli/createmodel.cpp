#include "createmodel.h"

#include <QDir>
#include <QDebug>

#include "linalg/pca.h"
#include "linalg/frequencymodulation.h"
#include "strain/videodataclip.h"
#include "strain/statisticalshapemodel.h"
#include "strain/longitudinalstrain.h"
#include "strain/shapenormalizer.h"

bool checkMatrix(const cv::Mat &mat)
{
    for (int r = 0; r < mat.rows; r++)
    {
        for (int c = 0; c < mat.cols; c++)
        {
            if (mat.at<float>(r,c) != mat.at<float>(r,c))
            {
                qDebug() << "NaN @" << r << c;
                return false;
            }
        }
    }

    return true;
}

#include <vector>

void minMax(const Points &points, float &minX, float &maxX, float &minY, float &maxY)
{
    minX = 1e300, maxX = -1e300, minY = 1e300, maxY = -1e300;
    for (P p: points)
    {
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;

        if (p.x < minX) minX = p.x;
        if (p.y < minY) minY = p.y;
    }
}

void preview(const Points &rawPoints, const Points &allPoints)
{
    float minX, maxX, minY, maxY;
    minMax(allPoints, minX, maxX, minY, maxY);

    Mat8 img = MatF::ones(500,500)*255;
    for (P p: allPoints)
    {
        P c((p.x - minX)/(maxX - minX)*500, (p.y - minY)/(maxY - minY)*500);
        cv::circle(img, c, 3, 0, 1, CV_AA);
    }

    //minMax(rawPoints, minX, maxX, minY, maxY);
    Points splineCtrlPoints;
    for (P p: rawPoints)
    {
        P c((p.x - minX)/(maxX - minX)*500, (p.y - minY)/(maxY - minY)*500);
        //cv::circle(img, c, 4, 0, -1, CV_AA);
        splineCtrlPoints.push_back(c);
    }
    Spline spline;
    spline.drawSpline(splineCtrlPoints, img, false, 0);

    cv::imshow("shape", img);
    cv::waitKey(0);
}

void CreateModel::create(const QString &dirPath, const QString &modelName)
{
    float freqStart = 1.0;
    float freqEnd = 3.0;
    float freqStep = 1.0;
    float phaseSteps = 3.0;
    float amplitude = 1.0;
    int pointsPerSegment = 10;
    int segments = 6;

    std::vector<VectorF> modValuesVector =
            FrequencyModulation::generateModulationValues(segments*pointsPerSegment + 1,
                                                          freqStart, freqEnd, freqStep, phaseSteps, amplitude);

    VectorF widths; widths.push_back(15); widths.push_back(20); widths.push_back(25);

    QString nameFilter = "*_metadata";
    QDir dir(dirPath, nameFilter);

    VectorOfShapes shapes;
    LongitudinalStrain dummyStrain(new ShapeNormalizerPass(), segments, pointsPerSegment);

    QFileInfoList files = dir.entryInfoList();
    foreach (const QFileInfo &fInfo, files)
    {
        cv::FileStorage mdStorage(fInfo.absoluteFilePath().toStdString(), cv::FileStorage::READ);
        VideoDataClipMetadata metaData(0);
        metaData.deserialize(mdStorage);
        qDebug() << fInfo.fileName() << ":" << metaData.rawShapes.count();

        foreach (const Points &rawShape, metaData.rawShapes.values())
        {
            //Points previewPoints = dummyStrain.getRealShapePoints(rawShape, 20);
            //preview(rawShape, previewPoints);

            for (float width : widths)
            {
                for (const VectorF &modValues : modValuesVector)
                {
                    shapes.push_back(dummyStrain.getRealShapePoints(rawShape, width, &modValues, 0));
                }
            }
        }
    }

    PCA *pca = new PCA();
    StatisticalShapeModel *model = new StatisticalShapeModel(pca, shapes);


    qDebug() << "PCA modes before selection:" << pca->getModes();
    pca->modesSelectionThreshold(0.99);
    qDebug() << "PCA modes after selection:" << pca->getModes();

    // check pca
    qDebug() << "PCA mean      OK:" << checkMatrix(pca->cvPca.mean);
    qDebug() << "PCA eigenvecs OK:" << checkMatrix(pca->cvPca.eigenvectors);
    qDebug() << "PCA eigenvals OK:" << checkMatrix(pca->cvPca.eigenvalues);


    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain strain(normalizer, segments, pointsPerSegment);

    cv::FileStorage resultStorage(modelName.toStdString(), cv::FileStorage::WRITE);
    strain.serialize(resultStorage);
}
