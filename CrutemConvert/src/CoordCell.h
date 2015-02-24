/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/

#ifndef EM_COORD_CELL_H
#define EM_COORD_CELL_H

#include <future>
#include <vector>

#include "EarthCoordSystem.h"
#include "StationListFormat.h"
#include "Temperature.h"
#include "Date.h"

/*
	Represents a coordinate grid's important data as well as working as a
	logical computational block.

	Each EMCoordCell has:
		Four Coordinate Points: (EMCoordRect)
			West, North, East, South

		An elevation grid (size determined at compile time).

		A vector of pointers to each station falling within the cell.

		A computed area value.

		A date range.

	EMCoordCell is not inherently thread safe, write accesses should be
	controlled and no read operations should be in flight during writes.
*/

class	EMCoordCell	{
public:
								EMCoordCell(const EMCoordRect&);
	virtual						~EMCoordCell();

	const	EMCoordRect&		Coordinates	() const;
			float				Area		() const;
			float				Elevation	() const;
			float				ElevationAt	(const EMCoordinate&);

			// Do we have the station?
			bool				Find		(const Station*) const;

			bool				Remove		(const Station*);

			bool				Insert		(Station*);
				// fails if the station does not belong

			int32				Count		() const;

			void				for_each	(std::function<void(Station*)>);

			EMDate				StartYear	() const;
			EMDate				EndYear		() const;

			EMTemperature		AverageFor	(EMDate);
			EMTemperature		AverageFor	(EMDate, EMDate);

private:
		std::pair<double, uint32>&
								_ElevGridForCoord(float, float);
		void					_RecalcElevAvg() const;

		std::vector<Station*>	fStations;

		std::vector<std::pair<double, uint32> >
								fElevationGrid;
	mutable float				fElevationAvg;
	mutable	bool				fElevAvgDirty;
		float					fArea;
		EMCoordRect				fCoordinates;

		EMDate					fStartYear;
		EMDate					fEndYear;
};


#endif //EM_COORD_CELL_H
