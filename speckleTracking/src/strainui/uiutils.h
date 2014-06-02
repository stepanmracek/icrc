#ifndef UIUTILS_H
#define UIUTILS_H

#include <QPixmap>
#include <QImage>

#include "linalg/common.h"

class UIUtils
{
public:
    static QPixmap Mat8ToQPixmap(const Mat8 &frame)
    {
        QImage image(frame.cols, frame.rows, QImage::Format_RGB32);
        for (int r = 0; r < frame.rows; r++)
        {
            for (int c = 0; c < frame.cols; c++)
            {
                int val = frame(r, c);
                int color = qRgb(val, val, val);
                image.setPixel(c, r, color);
            }
        }
        return QPixmap::fromImage(image);
    }
};

#endif // UIUTILS_H
