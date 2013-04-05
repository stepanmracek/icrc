#ifndef ANOTATIONDATA_H
#define ANOTATIONDATA_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "linalg/common.h"

class AnotationData
{
public:
    Points points;
    Mat8 image;
};

#endif // ANOTATIONDATA_H
