#ifndef ANOTATION_H
#define ANOTATION_H

#include <list>

#include "linalg/common.h"
#include "videodatabase.h"

/**
 * @brief Temporary class for anotating video data
 */
class Anotation
{
public:
    static VectorOfShapes anotateFromVideo(VideoDataBase &video);
    static Points anotateFromFrame(Mat8 &frame);
    static void showShape(Mat8 &frame, Points &shape);
};

#endif // ANOTATION_H
