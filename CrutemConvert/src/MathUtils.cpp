#include <math.h>
#include <stdlib.h>
#include <cstdint>
#include <cfloat>

#include "MathUtils.h"

#ifndef M_PI    // Windows issue...
#   define  M_PI 3.14159265358979323846
#endif

float	LMax(const float* floats, uint32 count)
{
	float max = FLT_MIN;
	for (int32 i = 0; i < count; ++i)
		if (max < floats[i])
			max = floats[i];

	return max;
}


float	LMin(const float* floats, uint32 count)
{
	float min = FLT_MAX;
	for (int32 i = 0; i < count; ++i)
		if (min > floats[i])
			min = floats[i];

	return min;
}


/*	Polygon Operations (2D)	*/
double	LCalculateArea(const EMPoint* points, int32 count)
{	// A special thanks to Rex Finley!
	if (count < 3 || points == NULL)	return 0;

	int32 i = 0, j = count - 1;	// [0], [n-1] happens first ;-)
	double area = 0;

	for (; i < count; ++i) {
		area += (points[i].x + points[j].x) * (-points[j].y - -points[i].y);
		j = i;
	}
	area /= 2;
	return area > 0 ? area : -area;;
}


EMPoint	LCalculateCenter(const EMPoint* points, int32 count)
{
	// A special thanks to Emile Cormier!
	EMPoint center;
	if (points == NULL)	return center;

	double	signedArea = 0,
			x0 = 0,
			y0 = 0,
			x1 = 0,
			y1 = 0,
			a = 0;

	int i = 0;
	for (; i < count - 1; ++i) {
		x0 = points[i].x;
		x1 = points[i+1].x;

		y0 = points[i].y;
		y1 = points[i+1].y;

		a = (x0 * y1) - (x1*y0);

		signedArea += a;
		center.x += (x0 + x1)*a;
		center.y += (y0 + y1)*a;
	}

	// DO last vertex
		x0 = points[i].x;
		x1 = points[0].x;

		y0 = points[i].y;
		y1 = points[0].y;

		a = (x0 * y1) - (x1*y0);

		signedArea += a;
		center.x += (x0 + x1)*a;
		center.y += (y0 + y1)*a;

		signedArea *= 0.5;
		center.x /= (6.0*signedArea);
		center.y /= (6.0*signedArea);

		return center;
}


double	LCalculateAreaOnSphere(const EMPoint* points, int32 count,
	double radius)
{
	return LCalculateArea(LSinusProject(points, count, radius), count);
}


EMPoint*	LSinusProject(const EMPoint* points, int32 count, double radius)
{	// looncraz
	if (points == NULL || count < 3 || radius == 0) return 0;
			// TODO: Is a negative radius workable? (convex sphere?)
	double dist = LRadians(radius);
	EMPoint *adjusted = new EMPoint[count];

	// flattening the polygon
	for (int i = 0; i < count; ++i) {
		adjusted[i].y = points[i].y * dist;	// simple enough ;-)
		adjusted[i].x = points[i].x * dist * cos(LRadians(points[i].y));
			// WHY do I feel like something is missing!?!
	}

	return adjusted;
}



float	LDistance(const EMPoint& from, const EMPoint& to,
	 float radius)	// Warning! Not throughly tested!
{
	float latDelta = LRadians(to.y - from.y);
	float lonDelta = LRadians(to.x - from.x);
	float fromLat = LRadians(from.y);
	float toLat = LRadians(to.y);

	float a = sin(latDelta/2) * sin(latDelta/2)
			+ sin(lonDelta /2) * sin(lonDelta/2)
			* cos(fromLat) * cos(toLat);

	return radius * (2 * atan2(sqrt(a), sqrt(1-a)));
}


double	LRadians(double degrees)
{
	return degrees * M_PI / 180.0;
}


double	LDegrees(double radians)
{
	return radians * 180/M_PI;
}



double	LInterpolate(double x, double y0, double x0,
					 double y1, double x1)
{
#if 0
	printf("Find value for day %.2f\n", x);
	printf("\tPrevious: %.2f @ %.2f\n", y0, x0);
	printf("\tNext: %.2f @ %.2f\n", y1, x1);
#endif
	return y0 + ((y1 - y0) * ((x - x0)/(x1 - x0)));
}



