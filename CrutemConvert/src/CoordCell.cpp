#include <algorithm>
#include <cmath>

#include "MathUtils.h"
#include "Rect.h"
#include "Point.h"
#include "Temperature.h"

#include "CoordCell.h"

#define ELEV_GRID_SZ	2

EMCoordCell	::	EMCoordCell(const EMCoordRect& poly)
	:
	fElevationGrid(ELEV_GRID_SZ * ELEV_GRID_SZ),
	fElevationAvg(0),
	fElevAvgDirty(false),
	fArea(0),
	fCoordinates(poly)
{
	EMPoint* pt = poly.ToPointArray();
	fArea = LCalculateArea(pt, 4);
	delete pt;
}


EMCoordCell	::	~EMCoordCell()
{
}


const	EMCoordRect&
EMCoordCell	::	Coordinates	() const
{
	return fCoordinates;
}


float
EMCoordCell	::	Area		() const
{
	return fArea;
}


float
EMCoordCell	::	Elevation	() const
{
	if (fElevAvgDirty)
		_RecalcElevAvg();
	return fElevationAvg;
}


float
EMCoordCell	::	ElevationAt	(const EMCoordinate & coord)
{
	const auto& grid = _ElevGridForCoord(coord.latitude, coord.longitude);
	return (grid.first / (double)grid.second);
}


bool
EMCoordCell	::	Find	(const Station* station) const
{
	return std::find(fStations.begin(), fStations.end(), station)
		!= fStations.end();
}


bool
EMCoordCell	::	Remove		(const Station* station)
{
	bool result = false;
	fStations.erase(
	std::remove_if(fStations.begin(), fStations.end(),
		[&result, station](const Station* elem){
		if (station == elem) {
			result = true;
			return true;
		}
		return false;
	}));

	if (result) {
		auto& elevGrid = _ElevGridForCoord(station->LAT, station->LON);
		elevGrid.first -= station->ELEV;
		elevGrid.second--;
	}

	return result;
}


bool
EMCoordCell	::	Insert		(Station* station)
{
	if (	station == nullptr
		|| 	fCoordinates.Contains(station->LAT, station->LON) == false)
		return false;

	fStations.push_back(station);

	// elevation grid
	auto& elevGrid = _ElevGridForCoord(station->LAT, station->LON);
	elevGrid.first += station->ELEV;
	elevGrid.second++;
	fElevAvgDirty = true;

	// start/end year
	if (fStartYear.IsYearValid() == false
		|| fStartYear.Year() > station->STARTYEAR)
		fStartYear = _year(station->STARTYEAR);

	if (fEndYear.IsYearValid() == false
		|| fEndYear.Year() > station->ENDYEAR)
		fEndYear = _year(station->ENDYEAR);

	return true;
}


int32
EMCoordCell	::	Count		() const
{
	return fStations.size();
}


void
EMCoordCell	::	for_each	(std::function<void(Station*)> func)
{
	for (auto& elem : fStations)
		func(elem);
}


EMDate
EMCoordCell	::	StartYear	() const
{
	return fStartYear;
}


EMDate
EMCoordCell	::	EndYear		() const
{
	return fEndYear;
}


EMTemperature
EMCoordCell	::	AverageFor	(EMDate date)
{
	if (date.IsValid() == false)
		return _kelvin(0.0);

	double accum = 0;
	int32 count = 0;

	for_each(	[&accum, &count, &date](Station* station){
		EMTemperature temp = station->AverageFor(date);
		if (temp.IsValid()) {
			accum += temp.toKelvin();
			count++;
		}
		// if the temp isn't valid, the station lacks the data
	});

	return _kelvin(accum / (double)count);
}


EMTemperature
EMCoordCell	::	AverageFor	(EMDate d1, EMDate d2)
{

	return _kelvin(0.0);
}



std::pair<double, uint32>&
EMCoordCell	::	_ElevGridForCoord(float lat, float lon)
{	// The grid size is fixed at compilation
	// lat & lon are guaranteed to be inside our bounds
	// ELEV_GRID_SZ x ELEV_GRID_SZ

	float gridTurn = 1.0/ELEV_GRID_SZ,
		x = lon	/ fCoordinates.Width(),
		y = lat / fCoordinates.Height();

		// guarantee a cell will be occupied
		if (x / ELEV_GRID_SZ <= gridTurn)
			x *= (ELEV_GRID_SZ - 1);
		else
			x *= ELEV_GRID_SZ;

		if (y / ELEV_GRID_SZ <= gridTurn)
			y *= (ELEV_GRID_SZ - 1);
		else
			y *= ELEV_GRID_SZ;

	int32 position = (y * ELEV_GRID_SZ) + x;
// TODO (use Build.h) ?
//	DASSERT((position >= 0 && position < ELEV_GRID_SZ * ELEV_GRID_SZ),
//		 "Internal checks failed to constrain lat/lon to grid");

	// decided to throw an exception for now...
	return fElevationGrid.at(position);
}


void
EMCoordCell	::	_RecalcElevAvg() const
{
	double accum = 0;

	for (auto elP : fElevationGrid)
		accum += (double)elP.first / (double)elP.second;

	fElevationAvg = accum / (ELEV_GRID_SZ * ELEV_GRID_SZ);
	fElevAvgDirty = false;
}



