#include "EarthCoordSystem.h"


EMCoordRect	::	EMCoordRect()
	:
	West(0),
	North(0),
	East(0),
	South(0)
	{}


EMCoordRect	::	EMCoordRect(const EMCoordRect& other)
	:
	West(other.West),
	North(other.North),
	East(other.East),
	South(other.South)
	{}	// we save the copy of _RSRV


EMCoordRect	::	~EMCoordRect()
{}


float
EMCoordRect	::	Width() const
{
	return East - West;
}


float
EMCoordRect	::	Height() const
{
	return South - North;
}


EMPoint*
EMCoordRect	::	ToPointArray() const
{
	EMPoint *points = new EMPoint[4];
	points[0] = EMPoint(West, South);
		points[1] = EMPoint(West, North);
	points[2] = EMPoint(East, North);
	points[3] = EMPoint(East, South);
	return points;
}


EMRect
EMCoordRect	::	ToRect() const
{	EMRect result(West, North, East, South);
	// EMRect should always have a non-negative origin
	result.OffsetBy(180, 90);
	return result;
}


void
EMCoordRect	::	PrintToStream(const char* prefix) const
{
	// We print out all corners for... reasons...
	printf("%sEMCoordRect {\n", prefix);
	printf("%s\tSouthWest:\t%.2f, %.2f\n",prefix, South, West);
	printf("%s\tNorthWest:\t%.2f, %.2f\n",prefix, North, West);
	printf("%s\tNorthEast:\t%.2f, %.2f\n",prefix, North, East);
	printf("%s\tSouthEast:\t%.2f, %.2f\n",prefix, South, East);
	printf("%s}\n", prefix);
}


bool
EMCoordRect	::	Contains (const EMCoordinate& coord) const
{
	return Contains(coord.latitude, coord.longitude);
}


bool
EMCoordRect	::	Contains	(float lat, float lon) const
{
	// If it falls on our eastern or southern edge or inside our
	// borders, it is ours, otherwise not.

	// TODO: make exception for north pole? Probably not needed...

	// The order of the checks are to optimize OoO branch prediction
	// and cache locality.  No, seriously.
	return 	(lat > West && lon > North)
		&&	(lat<= East	&& lon <= South);
}

