#include "batchstraintracker.h"

#include <QDir>
#include <QDebug>

#include "linalg/serialization.h"
#include "linalg/frequencymodulation.h"
#include "linalg/vecf.h"
#include "strain/videodataclip.h"
#include "strain/strainstatistics.h"
#include "strain/strainclassifier.h"
#include "strainui/dialogbeattobeat.h"

void BatchStrainTracker::extractStrains(const QString &inputDirPath, const QString &outputDirPath)
{
    QDir().mkpath(outputDirPath);
    // params
    int samplesPerBeat = 50;
    QString strainPath = "/home/stepo/Dropbox/projekty/icrc/dataDir/longstrain-fm-6-10";

    // initilize model
    PCA *pca = new PCA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, 0, 0);

    cv::FileStorage storage(strainPath.toStdString(), cv::FileStorage::READ);
    strain->deserialize(storage);

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    // load directory
    QStringList filter; filter << "*.mp4" << "*.avi" << "*.wmv";
    QDir dir(inputDirPath);
    QFileInfoList fileInfos = dir.entryInfoList(filter, QDir::Files, QDir::SortFlags(QDir::Name | QDir::IgnoreCase));

    // for each video
    foreach (const QFileInfo &info, fileInfos)
    {
        qDebug() << info.absoluteFilePath().toStdString().c_str();

        VideoDataClip clip(info.absoluteFilePath(), info.absoluteFilePath() + "_metadata");
        VideoDataClipMetadata *metadata = clip.getMetadata();

        QVector<VectorF> strains;

        // for each beat
        for(int i = 0; i < metadata->beatIndicies.count()-1; i++)
        {
            int beatStart = metadata->beatIndicies[i];
            int beatEnd = metadata->beatIndicies[i+1];
            Points initialShape = tracker->getStrain()->getRealShapePoints(metadata->rawShapes[beatStart], 20);

            // track
            ShapeMap shapeMap = tracker->track(&clip, beatStart, beatEnd, initialShape);
            StrainStatistics stats(strain, shapeMap.values().toVector().toStdVector());

            // resample
            VectorF strainValues = VecF::resample(stats.strain, samplesPerBeat);

            // serialize
            QString resultPath = outputDirPath + QDir::separator() + info.baseName() + "-" + QString::number(i);
            VecF::toFile(strainValues, resultPath);

            strains << strainValues;

            cv::FileStorage s(resultPath.toStdString()+".xml", cv::FileStorage::WRITE);
            stats.serialize(s);
            s.release();

            /*cv::FileStorage s2(resultPath.toStdString()+".xml", cv::FileStorage::READ);
            StrainStatistics stats2; stats2.deserialize(s2);

            qDebug() << "b2b        " << StrainStatistics::beatToBeatVariance(stats, stats2, 50);
            qDebug() << "b2b/segment" << QVector<float>::fromStdVector(StrainStatistics::beatToBeatVariancePerSegment(stats, stats2, 50));*/
        }


        VectorF mean;
        VectorF diff;
        for (int i = 0; i < samplesPerBeat; i++)
        {
            VectorF slice;
            for (int beat = 0; beat < strains.count(); beat++)
            {
                slice.push_back(strains[beat][i]);
            }

            diff.push_back(VecF::stdDeviation(slice));
            mean.push_back(VecF::meanValue(slice));
        }

        VecF::toFile(mean, outputDirPath + QDir::separator() + info.baseName() + "-mean");
        VecF::toFile(diff, outputDirPath + QDir::separator() + info.baseName() + "-std");
    }
}

void BatchStrainTracker::stats(const QString &extractedStainsDirPath)
{
    QMultiMap<QString, StrainStatistics> map = loadDirectoryWithStrains(extractedStainsDirPath, ".xml");
    QList<QString> persons = map.uniqueKeys();
    QVector<StrainStatistics::SegmentStatsResult> stats;

    foreach(const QString &person, persons)
    {
        QVector<StrainStatistics> strains = map.values(person).toVector();
        if (strains.size() > 3) strains.resize(3);
        if (strains.size() < 3) continue;

        qDebug() << " " << person << ":" << strains.count() << "beats";
        stats << StrainStatistics::segmentStatistics(strains, 100);

        //DialogBeatToBeat dlgb2b(strains);
        //dlgb2b.exec();
    }

    StrainStatistics::SegmentStatsResult mean = StrainStatistics::SegmentStatsResult::mean(stats);
    qDebug() << "mean variance" << mean.meanVariance;
    qDebug() << "mean correlation" << mean.meanCorrelation;
    qDebug() << "segment variance" << mean.segmentVariance;
    qDebug() << "segment correlationw" << mean.segmentCorrelation;
}

void BatchStrainTracker::exportValues(const QString &extractedStainsDirPath)
{
    QMultiMap<QString, StrainStatistics> map = loadDirectoryWithStrains(extractedStainsDirPath, ".xml");
    QList<QString> persons = map.uniqueKeys();

    foreach(const QString &person, persons)
    {
        QList<StrainStatistics> statsForPerson = map.values(person);
        for (int statCounter = 0; statCounter < statsForPerson.size(); statCounter++)
        {
            const StrainStatistics &strain = statsForPerson[statCounter];

            strain.saveValues(extractedStainsDirPath + QDir::separator() + person + "-" + QString::number(statCounter));
        }
    }
}

ShapeTracker * BatchStrainTracker::learn()
{
    QString rawShapesPath =  "/home/stepo/Dropbox/projekty/icrc/dataDir/rawControlPoints";
    VectorOfShapes rawShapes = Serialization::readVectorOfShapes(rawShapesPath);

    float freqStart = 1.0;
    float freqEnd = 3.0;
    float freqStep = 1.0;
    float phaseSteps = 3.0;
    float amplitude = 1.0;
    int pointsPerSegment = 10;
    int segments = 6;

    std::vector<VectorF> modValues =
            FrequencyModulation::generateModulationValues(segments*pointsPerSegment + 1,
                                                          freqStart, freqEnd, freqStep, phaseSteps, amplitude);
    int modValuesCount = modValues.size();

    //std::vector<VectorF> widthValues =
    //        WidthModulation::generateValues(pointsPerSegment*segments + 1, 1, 5, 1,
    //                                        10, 25.0f, 0.15f);
    //int widthValuesCount = widthValues.size();

    ShapeNormalizerPass *dummyNormalizer = new ShapeNormalizerPass();
    LongitudinalStrain dummyStrain(dummyNormalizer, segments, pointsPerSegment);

    VectorOfShapes shapes;
    foreach (const Points &controlPoints, rawShapes)
    {
        //for (int j = 0; j < widthValuesCount; j++)
        for (int width = 20; width <= 30; width += 10)
        {
            for (int i = 0; i < modValuesCount; i++)
            {
                Points shape = dummyStrain.getRealShapePoints(controlPoints, width, &modValues[i], 0); //, &widthValues[j]);
                shapes.push_back(shape);
            }
        }
    }

    PCA *pca = new PCA();
    //ICA *ica = new ICA();
    StatisticalShapeModel *shapeModel = new StatisticalShapeModel(pca, shapes);
    ShapeNormalizerBase *normalizer = new ShapeNormalizerIterativeStatisticalShape(shapeModel);
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, segments, pointsPerSegment);

    ListOfImageProcessing processing;
    StrainResultProcessingBase *postProcessing = new StrainResProcFloatingAvg(3);
    PointTrackerBase *pointTracker = new PointTrackerOpticalFlow(21);
    VectorF weights; weights.push_back(1.0f);
    ShapeTracker *tracker = new ShapeTracker(strain, processing, pointTracker, postProcessing, weights);

    return tracker;
}

// -------------------------------------------------

void BatchStrainTracker::evaluate(const QString &positives, const QString &negatives)
{
    QMultiMap<QString, VectorF> posDict = loadDirectoryWithStrains(positives);
    QMultiMap<QString, VectorF> negDict = loadDirectoryWithStrains(negatives);

    QMap<QString, int> desiredOutputs;
    foreach (const QString &k, posDict.uniqueKeys()) { desiredOutputs[k] = 1; }
    foreach (const QString &k, negDict.uniqueKeys()) { desiredOutputs[k] = -1; }

    int falsePositives = 0;
    int falseNegatives = 0;
    foreach (const QString &testKey, desiredOutputs.keys())
    {
        QVector<VectorF> posValues;
        foreach (const QString &key, posDict.uniqueKeys())
        {
            if (testKey == key) continue;
            posValues += posDict.values(key).toVector();
        }

        QVector<VectorF> negValues;
        foreach (const QString &key, negDict.uniqueKeys())
        {
            if (testKey == key) continue;
            negValues += negDict.values(key).toVector();
        }

        StrainClassifierSVM classifier;
        classifier.learn(posValues, negValues);

        QVector<VectorF> testValues;
        if (desiredOutputs[testKey] == 1)
            testValues = posDict.values(testKey).toVector();
        else
            testValues = negDict.values(testKey).toVector();

        foreach (const VectorF &v, testValues)
        {
            bool result = classifier.classify(v);
            int desired = desiredOutputs[testKey];
            bool success = ((result && desired == 1) || (!result && desired == -1));
            qDebug() << "   guessed:" << result << "success: " << success;
            //if (success) successCount++;*/

            if (desired == 1 && !result) falseNegatives++;
            if (desired == -1 && result) falsePositives++;
        }
    }
    qDebug() << "falseNegatives:" << falseNegatives * 100 / posDict.values().size()
             << "falsePositives:" << falsePositives * 100 / negDict.values().size();
}

QMultiMap<QString, VectorF> BatchStrainTracker::loadDirectoryWithStrains(const QString &path)
{
    QMultiMap<QString, VectorF>  result;
    QDir dir(path);
    qDebug() << "loading files from:" << dir.absolutePath();
    QFileInfoList fileInfos = dir.entryInfoList(QDir::Filters(QDir::NoDotAndDotDot | QDir::Files),
                                                QDir::SortFlags(QDir::Name | QDir::IgnoreCase));
    foreach (const QFileInfo &info, fileInfos)
    {
        QStringList items = info.baseName().split('-');
        QString key = items[0];
        bool ok;
        int beat = items[1].toInt(&ok);
        if (!ok) continue;

        VectorF strainValues = VecF::fromFile(info.absoluteFilePath());
        result.insert(key, strainValues);
        //qDebug() << " " << info.fileName() << key << beat << strainValues.size();
    }

    return result;
}

QMultiMap<QString, StrainStatistics> BatchStrainTracker::loadDirectoryWithStrains(const QString &path, const QString &statsFileSuffix)
{
    QMultiMap<QString, StrainStatistics>  result;
    QDir dir(path);
    qDebug() << "loading files from:" << dir.absolutePath();
    QStringList namefilters; namefilters << ("*" + statsFileSuffix);
    QFileInfoList fileInfos = dir.entryInfoList(namefilters,
                                                QDir::Filters(QDir::NoDotAndDotDot | QDir::Files),
                                                QDir::SortFlags(QDir::Name | QDir::IgnoreCase));
    foreach (const QFileInfo &info, fileInfos)
    {
        QStringList items = info.baseName().split('-');
        QString key = items[0];
        //bool ok;
        //int beat = items[1].toInt(&ok);
        //if (!ok) continue;

        StrainStatistics stats;
        cv::FileStorage storage(info.absoluteFilePath().toStdString(), cv::FileStorage::READ);
        stats.deserialize(storage);
        result.insert(key, stats);
        //qDebug() << " " << info.fileName() << key << beat << strainValues.size();
    }

    return result;
}
