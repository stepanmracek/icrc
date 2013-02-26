#include "procrustes.h"

#include <cassert>
#include <iostream>

#include "vecf.h"

/***
 * Procrustes analysis
 */
void Procrustes::procrustesAnalysis(std::vector<MatF> &vectors, bool scale, float eps, int maxIterations)
{
    //std::cout << "Procrustes analysis" << std::endl;

    int count = vectors.size();
    assert(count > 0);

    int len = vectors[0].rows;
    assert(len > 0);
    assert(len % 2 == 0);
    //int n = len / 2;

    // Translate each vector to the center of origin
    centralize(vectors);

    // Choose one example as an initial estimate of the mean shape
    // scale it so that |x| = 1 (if scalinf is enabled)
    MatF mean = scale ? VecF::normalized(vectors[0]) : vectors[0];
    MatF oldMean = mean.clone();
    //float oldDelta = 1e300;

    int iteration = 1;
    bool end = false;
    while (!end)
    {
        // Align all the shapes with the current estimate of the mean shape.
        for (int i = 0; i < count; i++)
        {
            if (scale)
            {
                ScaleAndRotateCoefs c = align(vectors[i], mean);
                rotateAndScale(vectors[i], c);
            }
            else
            {
                float theta = getOptimalRotation(vectors[i], mean);
                ScaleAndRotateCoefs c; c.theta = theta;
                rotateAndScale(vectors[i], c);
            }
        }

        // Re-estimate mean from aligned shapes
        mean = getMeanShape(vectors);

        // Apply constraints on the current estimate of the mean
        if (scale)
            VecF::normalize(mean);

        // If not converged, iterate
        MatF diff = mean-oldMean;
        float delta = VecF::sqrMagnitude(diff);

        if (delta <= eps || iteration > maxIterations)
            end = true;

        oldMean = mean.clone();
        //oldDelta = delta;

        if (iteration % 100 == 0)
            std::cout << " iteration: " << iteration << "; delta: " << delta << std::endl;

        iteration += 1;
    }
}

void Procrustes::translate(MatF &vector, const TranslationCoefs &coefs)
{
    int n = vector.rows/2;
    for (int i = 0; i < n; i++)
    {
        vector(i) += coefs.xt;
        vector(i+n) += coefs.yt;
    }
}

TranslationCoefs Procrustes::centralizedTranslation(const MatF &vector)
{
    float meanx = 0.0;
    float meany = 0.0;
    int n = vector.rows/2;

    for (int i = 0; i < n; i++)
    {
        meanx += vector(i);
        meany += vector(i+n);
    }
    meanx /= n;
    meany /= n;

    TranslationCoefs c;
    c.xt = -meanx;
    c.yt = -meany;
    return c;
}

void Procrustes::centralize(MatF &vector)
{
    TranslationCoefs c = centralizedTranslation(vector);
    translate(vector, c);
}

void Procrustes::centralize(std::vector<MatF> &vectors)
{
	int n = vectors.size();
	for (int i = 0; i < n; i++)
		centralize(vectors[i]);
}

void Procrustes::rotateAndScale(MatF &vector, const ScaleAndRotateCoefs &coefs)
{
    float sint = coefs.s * sin(coefs.theta);
    float cost = coefs.s * cos(coefs.theta);

    int n = vector.rows/2;
    for (int i = 0; i < n; i++)
    {
        float oldx = vector(i);
        float oldy = vector(i+n);
        float x = cost*oldx - sint*oldy;
        float y = sint*oldx + cost*oldy;

        vector(i) = x;
        vector(i+n) = y;
    }
}

void Procrustes::transformate(MatF &vector, const TransformationCoefs &coefs)
{
    int n = vector.rows/2;
    for (int i = 0; i < n; i++)
    {
        float oldx = vector(i);
        float oldy = vector(i+n);
        float x = coefs.a * oldx + -coefs.b * oldy;
        float y = coefs.b * oldx + coefs.a * oldy;

        vector(i) = x;
        vector(i+n) = y;
    }
}

float Procrustes::getOptimalRotation(const MatF &from, const MatF &to)
{
    int n = from.rows/2;
    float numerator = 0.0;
    float denumerator = 0.0;
    for (int i = 0; i < n; i++)
    {
        numerator += (from(i) * to(n+i) - from(n+i) * to(i));
        denumerator += (from(i) * to(i) + from(n+i) * to(n+i));
    }

    return atan(numerator/denumerator);
}

ScaleAndRotateCoefs Procrustes::align(const MatF &from, const MatF &to)
{
    MatF reference = to.clone();
    float referenceScale = 1.0/VecF::magnitude(reference);
    VecF::mul(reference, referenceScale);

    MatF vector = from.clone();
    float vectorScale = 1.0/VecF::magnitude(vector);
    VecF::mul(vector, vectorScale);

    float s = vectorScale/referenceScale;
    float theta = getOptimalRotation(vector, reference);
    ScaleAndRotateCoefs c(s, theta);

    return c;
}

MatF Procrustes::getMeanShape(const std::vector<MatF> &vectors)
{
	int n = vectors.size();
	MatF mean = MatF::zeros(vectors[0].rows, 1);
	for (int i = 0; i < n; i++)
	{
		mean += vectors[0];
	}
	mean = mean / ((float)n);

	return mean;
}
