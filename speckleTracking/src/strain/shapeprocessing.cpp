#include "shapeprocessing.h"

P ShapeProcessing::getCenter(Points &input)
{
    if (input.size() == 0) return P();

    float sumx = 0;
    float sumy = 0;

    int n = input.size();
    for (int i = 0; i < n; i++)
    {
        sumx += input[i].x;
        sumy += input[i].y;
    }

    return P(sumx/n, sumy/n);
}

Points ShapeProcessing::resize(Points &input, float scale, P center)
{
    Points result;
    int n = input.size();
    for (int i = 0; i < n; i++)
    {
        float dirx = input[i].x - center.x;
        float diry = input[i].y - center.y;

        float x = center.x + dirx*scale;
        float y = center.y + diry*scale;

        result.push_back(P(x,y));
    }

    return result;
}

Points ShapeProcessing::resize(Points &input, float scale)
{
    P center = getCenter(input);
    return resize(input, scale, center);
}

Points ShapeProcessing::outline(Points &input, float outlineWidth, P center)
{
    Points result;
    int n = input.size();
    for (int i = 0; i < n; i++)
    {
        float dirx = input[i].x - center.x;
        float diry = input[i].y - center.y;
        float mag = sqrt(dirx*dirx + diry*diry);

        float x = input[i].x + dirx/mag*outlineWidth;
        float y = input[i].y + diry/mag*outlineWidth;

        result.push_back(P(x,y));
    }

    return result;
}

Points ShapeProcessing::outline(Points &input, float outlineWidth)
{
    P center = getCenter(input);
    return outline(input, outlineWidth, center);
}

