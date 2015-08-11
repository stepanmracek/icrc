#include "testbeatdetection.h"

#include <QDir>
#include <QDebug>
#include <QStringBuilder>

#include "strain/videodataclip.h"
#include "strain/beatdetector.h"

void TestBeatDetection::test()
{
    BeatDetector::Settings settings(20, 15, BeatDetector::DetectionSource::Angle, BeatDetector::PrevFramePolicy::Prev);
    BeatDetector detector(settings);

    QString path = "/home/stepo/Dropbox/projekty/icrc/test3/";
    QDir dir(path, "*.mp4");
    QFileInfoList infolist = dir.entryInfoList();
    for (QFileInfo &i : infolist)
    {
        qDebug() << i.absoluteFilePath();
        VideoDataClip clip(i.absoluteFilePath(), i.absoluteFilePath() + "_metadata");

        qDebug() << clip.getMetadata()->beatIndicies;
        qDebug() << detector.detect(&clip, true);
    }
}


void TestBeatDetection::eval()
{
    QString path = "/home/stepo/Dropbox/projekty/icrc/test3/";
    QVector<int> spacings;   spacings   << 5 << 10 << 15 << 20;
    QVector<int> averagings; averagings << 5 <<  7 <<  9 << 11 << 13 << 15;
    QVector<BeatDetector::DetectionSource> sources; sources << BeatDetector::DetectionSource::Amplitude
                                                            << BeatDetector::DetectionSource::Angle
                                                            << BeatDetector::DetectionSource::CenterDirection;
    QStringList sourceNames; sourceNames << "Amplitude" << "Angle" << "CenterDirection";
    QVector<BeatDetector::PrevFramePolicy> policies; policies << BeatDetector::PrevFramePolicy::First
                                                              << BeatDetector::PrevFramePolicy::Prev;
    QStringList policyNames; policyNames << "First" << "Prev";

    QDir dir(path, "*.mp4");
    QFileInfoList infolist = dir.entryInfoList();
    QList<VideoDataClip *> clips;

    for (QFileInfo &i : infolist)
    {
        qDebug() << i.absoluteFilePath();
        clips << new VideoDataClip(i.absoluteFilePath(), i.absoluteFilePath() + "_metadata");
    }

    for (int sourceIndex = 0; sourceIndex < sources.size(); sourceIndex++)
    {
        for (int policyIndex = 0; policyIndex < policies.size(); policyIndex++)
        {
            qDebug() << sourceNames[sourceIndex] << policyNames[policyIndex];
            for (int spacing: spacings)
            {
                for (int averaging : averagings)
                {
                    BeatDetector::Settings settings(spacing, averaging, sources[sourceIndex], policies[policyIndex]);
                    BeatDetector detector(settings);

                    double diff = 0;
                    for (VideoDataClip *clip : clips)
                    {
                        auto compare = [] (QVector<int> &reference, QVector<int> &computed) {
                            double acc = 0;
                            for (int i = 1; i < reference.size(); i++)
                            {
                                if (i >= computed.size()) acc += pow(reference[1] - reference[0], 2);
                                else acc += pow(reference[i] - computed[i], 2);
                            }
                            acc /= (reference.size() - 1);
                            return acc;
                        };

                        QVector<int> indices = detector.detect(clip);
                        diff += compare(clip->getMetadata()->beatIndicies, indices);
                    }

                    qDebug() << spacing << averaging << diff;
                }
            }
        }
    }


    qDeleteAll(clips);
}
