#ifndef	EM_EARTH_COORD_SYSTEM_H
#define EM_EARTH_COORD_SYSTEM_H

#include "Point.h"
#include "Rect.h"

#include <stdio.h>

// 2-dimensional coordinates
typedef struct	EMCoordinate	{
	float		longitude;	//x
	float		latitude;	//y

	EMPoint		toPoint()
	{
		return (EMPoint(longitude, latitude));
	}

	EMCoordinate() : longitude(0), latitude(0) {}
	~EMCoordinate(){}
} EMCoordinate;


class	EMCoordRect {
public:
	// Organized clockwise
	float			West,	//	x, lon, left
					North,	//	y, lat, top
					East,	//	x, lon, right
					South;	//	y, lat, bottom

	float			_RSRV[4];

								EMCoordRect();
								EMCoordRect(const EMCoordRect&);

	virtual						~EMCoordRect();

			float				Width		() const;
			float				Height		() const;


			EMPoint*				ToPointArray() const;

			EMRect				ToRect		() const;

			void				PrintToStream(const char* = "") const;

			bool				Contains	(const EMCoordinate&) const;
			bool				Contains	(float lat, float lon) const;
};


#endif // EM_EARTH_COORD_SYSTEM_H

