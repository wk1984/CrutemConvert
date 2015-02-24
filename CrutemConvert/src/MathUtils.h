#ifndef L_MATH_UTILS_H
#define L_MATH_UTILS_H

#include "Point.h"
#include "StdTypedefs.h"

float	LMax(const float*, uint32 count);
float	LMin(const float*, uint32 count);

/*	Polygon Operations (2D)	*/
double	LCalculateArea(const EMPoint*, int32 count);
EMPoint	LCalculateCenter(const EMPoint*, int32 count);
double	LCalculateAreaOnSphere(const EMPoint*, int32, double radius);

// Convert spherical polygon into a flat representation
EMPoint*	LSinusProject(const EMPoint*, int32, double radius);

// Distance between two points on a sphere
float	LDistance(const EMPoint&, const EMPoint&, float radius);

double	LRadians(double degrees);
double	LDegrees(double radians);


double	LInterpolate(double position, double prevValue, double prevPosition,
					 double nextValue, double nextPosition);


#endif	//L_MATH_UTILS_H

