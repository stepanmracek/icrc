#ifndef SHAPEPROCESSING_H
#define SHAPEPROCESSING_H

#include "linalg/common.h"

class ShapeProcessing
{
public:
    static Points resize(Points &input, float scale);
    static Points resize(Points &input, float scale, P center);
    static Points outline(Points &input, float outlineWidth);
    static Points outline(Points &input, float outlineWidth, P center);
    static P getCenter(Points &input);
};

#endif // SHAPEPROCESSING_H
