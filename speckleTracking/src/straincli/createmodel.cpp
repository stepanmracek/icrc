#include "createmodel.h"

#include <QDir>
#include <QDebug>
#include <vector>

#include "linalg/pca.h"
#include "linalg/frequencymodulation.h"
#include "strain/videodataclip.h"
#include "strain/statisticalshapemodel.h"
#include "strain/longitudinalstrain.h"
#include "strain/shapenormalizer.h"

QList<QCommandLineOption> CreateModel::Settings::getCommandLineOptions()
{
    QList<QCommandLineOption> result;
    
    result << QCommandLineOption("freqStart", "Frequency start", "freqStart", "1.0");
    result << QCommandLineOption("freqEnd", "Frequency end", "freqEnd", "3.0");
    result << QCommandLineOption("freqStep", "Frequency step", "freqStep", "1.0");
    result << QCommandLineOption("phaseSteps", "Phase steps", "phaseSteps", "3.0");
    result << QCommandLineOption("amplitude", "Amplitude", "amplitude", "1.0");
    result << QCommandLineOption("pointsPerSegment", "Points per segment", "pointsPerSegment", "10");
    result << QCommandLineOption("segments", "Segments", "segments", "6");
    result << QCommandLineOption("widths", "Widths", "widths", "15;20;25");
    
    return result;
}

CreateModel::Settings CreateModel::Settings::get(const QCommandLineParser &parser)
{
    CreateModel::Settings result;
    result.freqStart = parser.value("freqStart").toFloat();
    result.freqEnd = parser.value("freqEnd").toFloat();
    result.freqStep = parser.value("freqStep").toFloat();
    result.phaseSteps = parser.value("phaseSteps").toFloat();
    result.amplitude = parser.value("amplitude").toFloat();
    result.pointsPerSegment = parser.value("pointsPerSegment").toInt();
    result.segments = parser.value("segments").toInt();
    
    result.widths.clear();
    QString widthsS = parser.value("widths");
    QStringList widthsList = widthsS.split(";", QString::SkipEmptyParts);
    for (const QString &w : widthsList) {
        result.widths.push_back(w.toInt());
    }
    
    return result;
}

void CreateModel::Settings::print() const
{
    QTextStream  s(stdout);
    s << "freqStart: " << freqStart << "\n";
    s << "freqEnd: " << freqEnd << "\n";
    s << "phaseSteps: " << phaseSteps << "\n";
    s << "amplitude: " << amplitude << "\n";
    s << "pointsPerSegment: " << pointsPerSegment << "\n";
    s << "segments: " << segments << "\n";
    
    s << "widths: ";
    for (const float &f: widths)
    {
        s << f << ";";
    }
    s << "\n";
}

namespace {
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
}

void CreateModel::create(const QString &dirPath, const QString &modelName, const Settings &s)
{
    s.print();
    
    QTextStream out(stdout);
    
    out << "Generating modulation values" << endl;
    std::vector<VectorF> modValuesVector =
            FrequencyModulation::generateModulationValues(s.segments*s.pointsPerSegment + 1,
                                                          s.freqStart, s.freqEnd, s.freqStep, s.phaseSteps, s.amplitude);

    QString nameFilter = "*_metadata";
    QDir dir(dirPath, nameFilter);

    VectorOfShapes shapes;
    LongitudinalStrain dummyStrain(new ShapeNormalizerPass(), s.segments, s.pointsPerSegment);

    out << "Loading metadata files from " << dirPath << endl;
    QFileInfoList files = dir.entryInfoList();
    out << "Loaded " << files.length() << " files" << endl;
    foreach (const QFileInfo &fInfo, files)
    {
        cv::FileStorage mdStorage(fInfo.absoluteFilePath().toStdString(), cv::FileStorage::READ);
        VideoDataClipMetadata metaData(0);
        metaData.deserialize(mdStorage);
        out << fInfo.fileName() << ": " << metaData.rawShapes.count() << endl;

        foreach (const Points &rawShape, metaData.rawShapes.values())
        {
            //Points previewPoints = dummyStrain.getRealShapePoints(rawShape, 20);
            //preview(rawShape, previewPoints);

            for (float width : s.widths)
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


    out << "PCA modes before selection: " << pca->getModes() << endl;
    pca->modesSelectionThreshold(0.99);
    out << "PCA modes after selection:" << pca->getModes() << endl;

    // check pca
    out << "PCA mean      OK: " << checkMatrix(pca->cvPca.mean) << endl;
    out << "PCA eigenvecs OK: " << checkMatrix(pca->cvPca.eigenvectors) << endl;
    out << "PCA eigenvals OK: " << checkMatrix(pca->cvPca.eigenvalues) << endl;


    ShapeNormalizerIterativeStatisticalShape *normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    LongitudinalStrain strain(normalizer, s.segments, s.pointsPerSegment);

    cv::FileStorage resultStorage(modelName.toStdString(), cv::FileStorage::WRITE);
    strain.serialize(resultStorage);
}
