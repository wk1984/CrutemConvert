#ifndef CC_STATION_CALCULATIONS_H
#define CC_STATION_CALCULATIONS_H


#include <string>
#include <vector>

#include "Date.h"
#include "EarthCoordSystem.h"
#include "StationListFormat.h"
#include "StdTypedefs.h"

using namespace std;

namespace	Calc {

const float	FAILURE = -99.9;

/*
	Output data format for most Calc::* functions
*/

class	Result	{
public:
								Result	();
								~Result	();

			double				Get		(const EMDate&) const;
			void				Set		(const EMDate&, double);

			void				SetFinal(double*);
	const	double*				GetFinal() const;

	const	EMDateRange&		DateRange	() const;

	// iteration

			void				foreach_final
							(function<void(int8, double&)>);

			void				foreach_final
							(function<void(int8, double)>) const;

			// foreach_[TimePeriod]
			void				foreach_day
							(function<void(const EMDate&, double&)>);

			void				foreach_month
							(function<void(const EMDate&, double&)>);

			void				foreach_year
							(function<void(uint32, double&)>);


			// const foreach
			void				foreach_day
							(function<void(const EMDate&, double)>) const;

			void				foreach_month
							(function<void(const EMDate&, double)>) const;

			void				foreach_year
							(function<void(uint32, double)>) const;


private:
			EMDateRange			fDRange;

	vector<pair<uint32, double[13]>>
								fData;

			double				fFinal[13];
};


/*
	Example:

		Station* station = ...;

		EMDate start(1880, 1, 1), end(1950, 12, 31);
		EMDateRange dRange(start, end);

		Calc::Result result;
		string error;

		error = Calc::Trend(station, result, dRange);
		if (error != "")	return ERROR(error);

		cout << "Station " << station->NAME << " annual trends:\n";

		result.foreach_year([](const EMDate& date, double val)
			{
				date.PrintToStream("\t", "\t: ");
				cout << val ; endl;
			});

*/

/*
	The Result from the following functions (and all in Calc) may not
	begin or end with the full range of dates available.  This is due
	to beginning and ending months often not have reliable information.
	In addition, each Result holds a "final" result which tallies the
	overall calculations.  In many cases, this may be all that is
	of interest.

	Averages:
		Output includes untouched averages for the months and then the
		average for the year (based on the real data for that year).
		Final holds the overall averages.
		This is mostly used to calculate the annual and overall monthly
		averages. If this data exists in the Station it will be ignored.

	Deviations:
		Calculates standard deviation for each month and year across
		the date range.  Final holds the overall deviations.

	Infill:
		Finds missing data and infers the most likely value based on
		deviations, neighboring months, and neighboring years. Final
		holds the resulting quality of the data, adjusted for error in
		interpolation.

	Trend:
		Calculates the decadal trend on a moving basis, month to month,
		year to year.  Final holds the overall decadal trends for the
		entire time series.
*/

string	Averages	(const Station*, Result&, EMDateRange = 0);
string	Deviations	(const Station*, Result&, EMDateRange = 0);
string	Infill		(const Station*, Result&, EMDateRange = 0);
string	Trend		(const Station*, Result&, EMDateRange = 0);
string	Variance	(const Station*, Result&, EMDateRange = 0);





/*
	Compare two stations

	For the overlapping date ranges month-by-month the stations are
	compared to each other.

	CDeviation returns the amount the first station deviates from the
		second station on a monthly and annual basis.

	CCorrelation maps correlations between two stations monthly and	anually.

	CTrend maps the difference in trends month-to-month and year-to-year.
*/

string	CDeviation	(const Station*, const Station*, Result&, EMDateRange = 0);
string	CCorrelation(const Station*, const Station*, Result&, EMDateRange = 0);
string	CTrend		(const Station*, const Station*, Result&, EMDateRange = 0);

/*
	Operations for groups of stations
*/

string	NaiveAvg	(const LStationList&, Result&);
string	WeightedAvg	(const LStationList&, Result&, double cellSize);
string	WeightedAvg	(const LStationList&, Result&, EMCoordRect cell);
string	Correlation	(const LStationList&, Result&);
string	Correlation	(const LStationList&, const LStationList&, Result&);



}; // end namespace


#endif // CC_STATION_CALCULATIONS_H
