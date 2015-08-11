#include "beatdetector.h"

#include <iomanip>

#include "linalg/vecf.h"

BeatDetector::BeatDetector(const Settings &settings, QObject *parent) :
    QObject(parent), settings(settings)
{

}

QVector<int> BeatDetector::detect(VideoDataClip *clip, bool debug)
{
    auto printIndicies = [] (const QVector<int> &indicies) -> void
    {
        for (auto i: indicies) std::cout << std::setw(5) << i << " ";
        std::cout << std::endl;
    };

    auto cos = [] (const P &v1, const P &v2)
    {
        return v1.dot(v2) / (sqrt(v1.dot(v1)) * sqrt(v2.dot(v2)));
    };

    if (debug)
    {
        printIndicies(clip->getMetadata()->beatIndicies);
    }

    QVector<int> result;
    CoordSystemBase *coord = clip->getMetadata()->getCoordSystem();
    Points points = coord->getGrid(settings.gridSpacing);
    Points tPoints = coord->transformPoints(points);
    int oldIndex = clip->index();

    std::vector<unsigned char> status(points.size());
    VectorF err(points.size());
    Points tNextPoints(points.size());

    VectorF amplitude, angle, centerDirection;

    Mat8 prevFrame, nextFrame, preview;
    clip->setIndex(0);
    clip->getNextFrame(prevFrame);
    Mat8 tPrevFrame = coord->transform(prevFrame);
    P center(tPrevFrame.cols/2, tPrevFrame.rows/2);
    while (clip->getNextFrame(nextFrame))
    {
        Mat8 tNextFrame = coord->transform(nextFrame);
        cv::calcOpticalFlowPyrLK(tPrevFrame, tNextFrame, tPoints, tNextPoints, status, err);

        if (debug) preview = tNextFrame.clone();

        double ampl = 0;
        double angl = 0;
        double cosD = 0;
        int count = 0;
        for (size_t i = 0; i < points.size(); i++)
        {
            if (status[i] == 0) continue;

            P p = tNextPoints[i] - tPoints[i];
            P toCenter = center - tPoints[i];
            ampl += sqrt(p.ddot(p));
            angl += atan2(p.y, p.x);
            cosD += cos(p, toCenter);

            count++;

            if (debug)
            {
                cv::circle(preview, tPoints[i], 2, 255, 1, CV_AA);
                cv::line(preview, tPoints[i], tNextPoints[i], 255, 1, CV_AA);
            }
        }

        ampl /= count;
        angl /= count;
        cosD /= count;
        amplitude.push_back(ampl);
        angle.push_back(angl);
        centerDirection.push_back(cosD);

        if (debug)
        {
            cv::imshow("preview", preview);
            cv::waitKey(1);
        }

        if (settings.prevFramePolicy == PrevFramePolicy::Prev)
            tNextFrame.copyTo(tPrevFrame);
    }

    auto removeDirectComponent = [] (VectorF &vector)
    {
        float mean = VecF::meanValue(vector);
        std::transform(vector.begin(), vector.end(), vector.begin(), [&mean] (float val) { return val-mean; });
    };

    amplitude = VecF::floatingAverage(amplitude, settings.averagingKernelSize);
    removeDirectComponent(amplitude);

    angle = VecF::floatingAverage(angle, settings.averagingKernelSize);
    removeDirectComponent(angle);

    centerDirection = VecF::floatingAverage(centerDirection, settings.averagingKernelSize);
    removeDirectComponent(centerDirection);

    VectorF spectrum;
    switch (settings.detectionSource)
    {
    case DetectionSource::Amplitude:
        cv::dft(amplitude, spectrum);
        break;
    case DetectionSource::Angle:
        cv::dft(angle, spectrum);
        break;
    case DetectionSource::CenterDirection:
        cv::dft(centerDirection, spectrum);
        break;
    default:
        throw std::runtime_error("Unsupported detectionSource");
    }

    VectorF psd(spectrum.size());
    std::transform(spectrum.begin(), spectrum.end(), psd.begin(), [] (float val) { return  pow(val, 2); });

    auto createPlot = [] (const VectorF &vector, int height) {
        Mat8 result = Mat8::ones(height, vector.size()) * 255;
        auto minmax = std::minmax_element(vector.begin(), vector.end());
        float minVal = *minmax.first;
        float maxVal = *minmax.second;
        float delta = maxVal-minVal;
        minVal -= delta/5.0f;
        maxVal += delta/5.0f;
        delta = maxVal-minVal;

        auto toY = [&] (double val) { return height - height*(val-minVal)/delta; };

        if (maxVal > 0 && minVal < 0)
            cv::line(result, P(0, toY(0)), P(vector.size(), toY(0)), 127);
        for (size_t i = 0; i < vector.size() - 1; i++)
        {
            P p1(i, toY(vector[i]));
            P p2(i+1, toY(vector[i+1]));
            cv::line(result, p1, p2, 0, 1, CV_AA);
        }

        return result;
    };

    float maxPsd = VecF::maxIndex(psd);
    float maxFrequency = maxPsd / (angle.size()) * 0.5; // * 0.5f) * 0.5f;
    float period = 1.0f / maxFrequency;
    int firstBeat = clip->getMetadata()->beatIndicies.size() > 0 ? clip->getMetadata()->beatIndicies.first() : 0;

    if (debug)
    {
        Mat8 amplitudeImage = createPlot(amplitude, 100);
        Mat8 angleImage = createPlot(angle, 10*M_PI);
        Mat8 directionImage = createPlot(centerDirection, 100);
        Mat8 spectrumImage = createPlot(spectrum, 200);
        Mat8 psdImage = createPlot(psd, 200);

        for (float x = firstBeat; x < angleImage.cols; x += period)
        {
            cv::line(angleImage, P(x, 0), P(x, angleImage.rows), 127);
            cv::line(amplitudeImage, P(x, 0), P(x, amplitudeImage.rows), 127);
            cv::line(directionImage, P(x, 0), P(x, directionImage.rows), 127);
        }

        cv::imshow("amplitude", amplitudeImage);
        cv::imshow("angle", angleImage);
        cv::imshow("centerDirection", directionImage);
        cv::imshow("spectrumImage", spectrumImage);
        cv::imshow("psdImage", psdImage);
    }


    for (int index = firstBeat; index < clip->size(); index += period)
    {
        result << index;
    }

    if (debug)
    {
        printIndicies(result);
        cv::waitKey(0);
    }

    clip->setIndex(oldIndex);
    return result;
}
