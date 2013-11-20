#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <list>
#include <cfloat>
#include <QMap>

/**
 * @brief Matrix of unsigned char values. Usefull for greyscale image data
 */
typedef cv::Mat_<unsigned char> Mat8;

/**
 * @brief Matrix of float values.
 */
typedef cv::Mat_<float> MatF;

/**
 * @brief Point of a shape
 */
typedef cv::Point2f P;

/**
 * @brief Shape
 */
typedef std::vector<P> Points;

/**
 * @brief Vector of floats;
 */
typedef std::vector<float> VectorF;

/**
 * @brief Vector of shapes
 */
typedef std::vector<Points> VectorOfShapes;

/**
 * @brief Map from index to shape
 */
typedef QMap<int, Points> ShapeMap;

/**
 * @brief Vector of images
 */
typedef std::vector<Mat8> VectorOfImages;

/**
 * @brief Vector of floating point matricies
 */
typedef std::vector<MatF> VectorOfMatricies;

/**
 * @brief The Common class. Contains the most commnon static helper methods
 */
class Common
{
public:

    /**
     * @brief Converts Shape representation to OpenCV matrix
     * @param points input shape
     * @return Converted shape. The shape ((x1,y1),...,(xn,yn)) is converted to column matrix [x1,...,xm,y1,...,yn]
     */
    static MatF pointsToMatF(const Points &points)
	{
		int n = points.size();
		MatF mat(n*2, 1);
		for(int i = 0; i < n; i++)
		{
			mat(i) = points[i].x;
			mat(n+i) = points[i].y;
		}
		return mat;
	}

    /**
     * @brief Converts column matrix to the shape
     * @param mat input matrix
     * @return Converted matrix. The column matrix [x1,...,xm,y1,...,yn] is converted to shape ((x1,y1),...,(xn,yn))
     */
	static Points matFToPoints(MatF &mat)
	{
		Points points;
		int n = mat.rows/2;
		for (int i = 0; i < n; i++)
		{
			cv::Point2f p(mat(i), mat(n+i));
			points.push_back(p);
		}
		return points;
	}

    /**
     * @brief Converts the vector of shapes into one single column vector
     * @param shapes vector of points/shapes
     * @return Resulting matrix. [shape_1_x1,shape_1_y1,shape_1_x2,shape_1_y2,...,shape_1xn,shape_1ym,shape_2,...,shape_m]
     */
    static MatF pointsToMatF(const std::vector<Points> &shapes)
    {
        int m = shapes.size();
        assert(m > 0);
        int n = shapes[0].size();

        MatF result(m*n*2, 1);
        int r = 0;
        for (int shapeIndex = 0; shapeIndex < m; shapeIndex++)
        {
            const Points &shape = shapes[shapeIndex];
            for (int pointIndex = 0; pointIndex < n; pointIndex++)
            {
                const P &p = shape[pointIndex];
                result(r) = p.x;
                r++;
                result(r) = p.y;
                r++;
            }
        }

        return result;
    }

    /**
     * @brief Converts the input matrix to the vector of individual shapes (each shape is vector of points)
     * @param mat input column matrix
     * @param shapeCount number of resulting shapes
     * @return returns the vector of shapes
     */
    static std::vector<Points> matFToPoints(MatF &mat, int shapeCount)
    {
        assert(shapeCount > 0);
        assert((mat.rows/2) % shapeCount == 0);
        int n = mat.rows / shapeCount / 2;

        std::vector<Points> result;
        int r = 0;
        for (int shapeIndex = 0; shapeIndex < shapeCount; shapeIndex++)
        {
            Points shape;
            for (int pointIndex = 0; pointIndex < n; pointIndex++)
            {
                P p;
                p.x = mat(r);
                r++;
                p.y = mat(r);
                r++;
                shape.push_back(p);
            }
            result.push_back(shape);
        }

        return result;
    }

    /**
     * @brief Converts the column vectors into single matrix
     * @param vectors input vectors
     * @return Converted matrix. N input column vectors of length N is converted into N*M matrix
     */
    static MatF columnVectorsToDataMatrix(const std::vector<MatF> &vectors)
	{
		int cols = vectors.size();
		int rows = vectors[0].rows;

		MatF data(rows, cols);
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				data(r,c) = vectors[c](r);
			}
		}

		return data;
	}

    /**
     * @brief The Euclidean distance between two points
     * @param p1 first point
     * @param p2 second point
     * @return The Euclidean distance between two points
     */
    static float eucl(const P &p1, const P &p2)
    {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        return sqrt(dx*dx + dy*dy);
    }

    /**
     * @brief Determines the presence of NaN value in matrix
     * @param m input matrix
     * @return Returns true if input matrix contains NaN, otherwise false
     */
    static bool matrixContainsNan(MatF &m)
    {
        int rows = m.rows;
        int cols = m.cols;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                double val = m(r,c);
                if (val != val)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Calculates absolute sum of matrix elements
     * @param m input matrix
     * @return Absolute sum of matrix elements
     */
    static float absSum(MatF &m)
    {
        float sum = 0;
        for (int r = 0; r < m.rows; r++)
        {
            for (int c = 0; c < m.cols; c++)
            {
                sum += fabs(m(r,c));
            }
        }
        return sum;
    }

    /**
     * @brief Gets min and max values
     * @param m input matrix
     * @param min min is stored here
     * @param max max is stored here
     * @return returns false if matrix contains NAN, otherwise true
     */
    static bool getMinMax(MatF &m, float &min, float &max)
    {
        min = FLT_MAX;
        max = -FLT_MAX;
        int rows = m.rows;
        int cols = m.cols;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                double val = m(r,c);
                if (val != val)
                    return false;

                if (val > max) max = val;
                if (val < min) min = val;
            }
        }
        return true;
    }

    /**
     * @brief Prints matrix content on std::cout
     * @param m input matrix
     */
    static void printMatrix(MatF &m)
    {
        for (int r = 0; r < m.rows; r++)
        {
            for (int c = 0; c < m.cols; c++)
            {
                std::cout << m(r, c) << " ";
            }
            std::cout << std::endl;
        }
    }

    /**
     * @brief Creates a floating point submatrix window from input source
     * @param src Input 8-bit image
     * @param centerX x-center of window
     * @param centerY y-center of window
     * @param windowSize Size of the resulting window
     * @param result The resulting matrix. Should be initializated and the size should be windowSize^2
     */
    static void createWindow(Mat8 &src, int centerX, int centerY, int windowSize, MatF &result)
    {
        assert(windowSize >= 1);
        assert(windowSize % 2 == 1);

        int delta = windowSize / 2;

        int resultX = 0;
        int resultY = 0;
        for (int y = centerY-delta; y <= centerY+delta; y++)
        {
            for (int x = centerX-delta; x <= centerX+delta; x++)
            {
                if (y >= 0 && y < src.rows && x >= 0 && x < src.cols)
                    result(resultY, resultX) = src(y,x);
                else
                    result(resultY, resultX) = 0.0f;

                resultX++;
            }
            resultX = 0;
            resultY++;
        }

        return;
    }

    /**
     * @brief Calculate deltas of input vector
     * @param input Input Vector
     * @return Resulting deltas. The result has the same length as the input.
     */
    static VectorF deltas(VectorF &input)
    {
        VectorF result;
        int n = input.size();
        if (n == 0) return result;

        for (int i = 0; i < n; i++)
        {
            int prev = (i == 0) ? 0 : i-1;
            int next = (i == (n-1)) ? i : i+1;
            float delta = input[next] - input[prev];
            if (i > 0 && i < (n-1)) delta /= 2.0;
            result.push_back(delta);
        }
        return result;
    }

    /**
     * @brief Converts vector of shapes QMap
     * @param shapes input vector of shapes
     * @return resulting map, first key is 0
     */
    static ShapeMap vectorOfShapesToMap(VectorOfShapes &shapes)
    {
        QMap<int, Points> result;
        int n = shapes.size();
        for (int i = 0; i < n; i++)
        {
            result[i] = shapes[i];
        }
        return result;
    }

    /**
     * @brief Converts the input map of sheapes to the vector
     * @param map input map
     * @return resulting vector
     */
    static VectorOfShapes MapToVectorOfShapes(ShapeMap &map)
    {
        VectorOfShapes result;
        result.reserve(map.size());
        ShapeMap::Iterator i = map.begin();
        while (i != map.end())
        {
            result.push_back(i.value());
            ++i;
        }
        return result;
    }

    /**
     * @brief Normalize components of the input vector, such that sum of its components is 1
     * @param w Input column vector
     */
    static void normalizeSumOne(MatF &w)
    {
        int n = w.rows;
        assert(n >= 1);
        assert(w.cols == 1);

        float sum = 0.0;
        for (int i = 0; i < n; i++)
        {
            float val = w(i);
            if (val < 0.0)
            {
                val = 0.0;
                w(i) = 0.0;
            }
            sum += val;
        }

        for (int i = 0; i < n; i++)
            w(i) = w(i)/sum*n;
    }
};

#endif // COMMON_H
