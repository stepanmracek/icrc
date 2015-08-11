#ifndef COORDSYSTEM_H
#define COORDSYSTEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <list>

#include "linalg/common.h"

/**
 * @brief Base class for transfromation of input image data
 */
class CoordSystemBase : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    CoordSystemBase(QObject *parent = 0) : QObject(parent) { }

    /**
     * @brief Transform input image data
     * @param input Input image data
     * @return Transformed image data
     */
    virtual Mat8 transform(const Mat8 &input) = 0;

    /**
     * @brief Transform input point
     * @param input Input point
     * @return Transformed point
     */
    virtual P transform(P input) = 0;

    /**
     * @brief Transform input points
     * @param inputPoints Vector of input points
     * @return Transformed points
     */
    Points transformPoints(const Points &inputPoints)
    {
        Points result;
        int n = inputPoints.size();
        for (int i = 0; i < n; i++)
        {
            result.push_back(transform(inputPoints[i]));
        }
        return result;
    }

    /**
     * @brief Transform ionput shapes
     * @param inputShapes Vector of input shapes
     * @return Transformed shapes
     */
    VectorOfShapes transformShapes(const VectorOfShapes &inputShapes)
    {
        VectorOfShapes result;
        for (VectorOfShapes::const_iterator it = inputShapes.begin(); it != inputShapes.end(); ++it)
        {
            result.push_back(transformPoints(*it));
        }
        return result;
    }

    /**
     * @brief Back-transform input point
     * @param input Input point
     * @return Back-transformed point
     */
    virtual P backTransform(P input) = 0;

    /**
     * @brief Back-transform input points
     * @param inputPoints Input points
     * @return Back-transformed-points
     */
    Points backTransformPoints(Points &inputPoints)
    {
        Points result;
        int n = inputPoints.size();
        for (int i = 0; i < n; i++)
        {
            result.push_back(backTransform(inputPoints[i]));
        }
        return result;
    }

    /**
     * @brief Get equaly distanced points within the coord system area
     * @param spacing Spacing between points
     * @return Equaly spaced points
     */
    virtual Points getGrid(int spacing) const = 0;

    /**
     * @brief Draws representation of coord system
     * @param scene Scene where new graphics items are added
     * @return List of added graphics items
     */
    virtual QList<QGraphicsItem*> draw(QGraphicsScene *scene) = 0;

    enum Types
    {
        TypeRadial = 0,
        TypePass = 1,
        TypeROI = 2
    };

    virtual Types type() = 0;

    virtual CoordSystemBase *clone() = 0;
};

/**
 * @brief Empty coord system transformation
 */
class CoordSystemPass : public CoordSystemBase
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    CoordSystemPass(QObject *parent = 0) : CoordSystemBase(parent) { }

    Mat8 transform(const Mat8 &input) { return input; }
    P transform(P input) { return input; }
    P backTransform(P input) { return input; }
    QList<QGraphicsItem*> draw(QGraphicsScene *) { return QList<QGraphicsItem*>(); }
    Types type() { return TypePass; }
    CoordSystemBase *clone() { return new CoordSystemPass(); }
    Points getGrid(int /*spacing*/) const { return Points(); }
};

/**
 * @brief Radial coordinate system
 */
class CoordSystemRadial : public CoordSystemBase
{
    Q_OBJECT

public:
    P center; //, arcStart, arcEnd;
    float startDistance, endDistance;
    int resultMatCols, resultMatRows;

    float angleStart, angleStep, angleEnd, distanceStep, dAngle, dDistance;

public:

    /**
     * @brief Default constructor
     * @param parent Parent object in Qt hierarchy
     */
    CoordSystemRadial(QObject *parent = 0);

    /**
     * @brief CoordSystemRadial
     * @param center
     * @param arcStart
     * @param arcEnd
     * @param startDistance
     * @param resultMatCols
     * @param resultMatRows
     * @param parent
     */
    CoordSystemRadial(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows, QObject *parent = 0);

    /**
     * @brief CoordSystemRadial
     * @param center
     * @param startDistance
     * @param endDistance
     * @param angleStart
     * @param angleEnd
     * @param resultMatCols
     * @param resultMatRows
     * @param parent
     */
    CoordSystemRadial(P center, float startDistance, float endDistance, float angleStart, float angleEnd,
                      int resultMatCols, int resultMatRows, QObject *parent = 0);

    /**
     * @brief Copy constructor
     * @param src Source
     */
    CoordSystemRadial(CoordSystemRadial *src);

    /**
     * @brief Init from another radial coord system
     * @param src Source system
     */
    void init(CoordSystemRadial *src);

    /**
     * @brief init Init coord system
     * @param center
     * @param startDistance
     * @param endDistance
     * @param angleStart
     * @param angleEnd
     * @param resultMatCols
     * @param resultMatRows
     */
    void init(P center, float startDistance, float endDistance, float angleStart, float angleEnd, int resultMatCols, int resultMatRows);

    /**
     * @brief init
     * @param center
     * @param arcStart
     * @param arcEnd
     * @param startDistance
     * @param resultMatCols
     * @param resultMatRows
     */
    void init(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows);

    Mat8 transform(const Mat8 &src);
    P transform(P input);
    P backTransform(P input);

    static void AnotateAngleDistance(Mat8 &src, P &center, P &arcStart, P &arcEnd);

    QList<QGraphicsItem*> draw(QGraphicsScene *scene);

    Types type() { return TypeRadial; }

    CoordSystemBase *clone();

    Points getGrid(int spacing) const;
};

/**
 * @brief ROI-based coord system transformation
 */
class CoordSystemROI : public CoordSystemBase
{
    Q_OBJECT
public:
    cv::Rect roi;

    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    CoordSystemROI(QObject *parent = 0);
    CoordSystemROI(const cv::Rect roi, QObject *parent = 0);
    CoordSystemROI(CoordSystemROI *src);

    Mat8 transform(const Mat8 &input);
    P transform(P input);
    P backTransform(P input);
    QList<QGraphicsItem*> draw(QGraphicsScene * scene);
    Types type() { return TypeROI; }
    CoordSystemBase *clone() { return new CoordSystemROI(this); }
    Points getGrid(int spacing) const;
};

#endif // COORDSYSTEM_H
