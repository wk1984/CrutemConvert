#include "StationListFormat.h"

#include <string>
#include <stdio.h>

#include "Date.h"
#include "MathUtils.h"
#include "StdTypedefs.h"


YearData	::	YearData()
	:
	VALID(false),
	AVG(0)
{	}


//#pragma mark Station


Station	::	Station()
	:
	ID(0),
	DATA(nullptr)
{
	NAME[127] = '\0';
	COUNTRY[63] = '\0';
}


Station	::	~Station()
{
	delete[] DATA;
}


void
Station	::	PrintToStream() const
{
	printf("\r\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\r");
	printf("Station (%lu) {\n", ID);
	printf("\tCOORDS    : %.1f, %.1f\n", LAT, LON);
	printf("\tELEV      : %li meters\n", ELEV);
	printf("\tNAME      : %s\n", NAME);
	printf("\tCOUNTRY   : %s\n", COUNTRY);
	printf("\tFIRST YEAR: %li\n", STARTYEAR);
	printf("\tLAST YEAR : %li\n", ENDYEAR);
	printf("\tQUALITY   : %.2f\n", QUALITY);

	printf("\t\tYEAR  JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT");
	printf("  NOV  DEC  AVG\n");
	int32 count = ENDYEAR - STARTYEAR;
	for (int32 i = 0; i < count; ++i) {
		YearData& yd = DATA[i];
		printf("\t\t%lu %4.1f %4.1f %4.1f %4.1f %4.1f %4.1f %4.1f %4.1f %4.1f %4.1f",
			yd.YEAR,
			yd.JAN,		yd.FEB,		yd.MAR,		yd.APR,		yd.MAY,
			yd.JUN,		yd.JUL,		yd.AUG,		yd.SEP,		yd.OCT);
		printf(" %4.1f %4.1f %4.1f\n", yd.NOV, yd.DEC, yd.AVG);
	}

	printf("\tAVERAGES  :");
	printf("  ");
	for (int32 i = 0; i < 12; ++i)
	printf(" %.1f", AVERAGES[i]);
	printf("\n");
	printf("}\n");
}


EMDate
Station	::	StartYear	() const
{
	return _year(STARTYEAR);
}


EMDate
Station	::	EndYear		() const
{
	return _year(ENDYEAR);
}


EMTemperature
Station	::	AverageFor	(EMDate date, bool ignoreDay)
{
	float temp = -99.9;
	YearData* yd = nullptr;

	if (date.IsYearValid()
		&& date.Year() > STARTYEAR
		&& date.Year() < ENDYEAR)
		yd = &DATA[date.Year() - STARTYEAR];

	if (date.IsMonthValid()) {
		if (yd != nullptr) {	// we got the pointer for the year!
			switch (date.Month()) {
				case 1: temp = yd->JAN; break;
				case 2: temp = yd->FEB; break;
				case 3: temp = yd->MAR; break;
				case 4: temp = yd->APR; break;
				case 5: temp = yd->MAY; break;
				case 6: temp = yd->JUN; break;
				case 7: temp = yd->JUL; break;
				case 8: temp = yd->AUG; break;
				case 9: temp = yd->SEP; break;
				case 10: temp = yd->OCT; break;
				case 11: temp = yd->NOV; break;
				case 12: temp = yd->DEC; break;
			}
		} else {
			// we only want the month's average
			temp = AVERAGES[date.Month()];
		}
	} // END MONTH

		// TODO: try to guess anyway?
		// I think I'd rather infill rather than silently guess...
	if (temp < -99.0)	// Can't calculate day average without month
		return 0.0_kelvin;


	if (!ignoreDay && date.IsDayValid()) {
		// Using linear interpolation to estimate the temperature for
		// a given day in a month based on neighboring months.
		// Accuracy isn't 100%, but it's pretty good on average.

		// This is to create smooth transitions in animations, mostly.

		EMDate prevMonth = date - 1_month;
		EMDate nextMonth = date + 1_month;

		prevMonth << _day(prevMonth.DaysInMonth()/2);
		nextMonth << _day(nextMonth.DaysInMonth()/2);

		EMDate midDate = date;
			midDate << _day(date.DaysInMonth()/2);

		float	thisDay = date.DaysFrom(prevMonth),
				middleDay = prevMonth.DaysInMonth();

		float	prevTemp = AverageFor(prevMonth).toCelsius(),
				nextTemp = AverageFor(nextMonth).toCelsius(),
				result = temp;

		if (prevTemp < -99.0 || nextTemp < -99.0)
			return _celsius(temp);

		if (date.Day() > date.DaysInMonth()/2) {
			// second half of month
			result = LInterpolate(thisDay, temp, middleDay, nextTemp,
				nextMonth.DaysFrom(prevMonth));
		}
		else {
			// first half of month
			result = LInterpolate(thisDay, prevTemp, 1, temp, middleDay);
		}
		temp = result;
	}

	return _celsius(temp);
}


EMTemperature
Station	::	AverageFor	(EMDate from, EMDate to, bool ignoreDays)
{
	// This is a potentially heavy-hitting function.
	// We will run through the averages at the highest resolution
	// we can in order to not bias results.

	// This will call our sister function and accumulate the results.

	EMDate scanResolution = 1_month;

	if (!ignoreDays)
		scanResolution = 3_day;

	double accum = 0.0;
	float count = 0;

	EMTemperature temp;

	while ( from < to ) {
		temp = AverageFor(from, ignoreDays);
		if (temp.IsValid()) {
			accum += temp.toCelsius();
			count += 1;
		}
		from += scanResolution;
	}

	if (!ignoreDays) {
		// pick up any dangling days
		from -= scanResolution;
		temp = AverageFor(to, ignoreDays);

		if (temp.IsValid()) {
			// adjust value of temperature on last date.
			count += (1 - ((float)from.DaysFrom(to)
						/ (float)scanResolution.Day()));
			accum += temp.toCelsius();
		}
	}

	return _celsius(accum / count);
}


// #pragma mark StationListHeader

StationListHeader	::	StationListHeader()
	:	DATA_FORMAT_VERSION(1),
		STATION_COUNT(0),
		STATIONS(NULL)
{
	std::string cook("emslFunSize");
	for (int32 i = 0; i < 11; ++i)
		COOKIE[i] = cook[i];

	COOKIE[11] = '\0';
	SOURCE_NAME[127] = '\0';
	SOURCE_VERS[63] = '\0';
	SOURCE_BUILDER[127] = '\0';
}

StationListHeader	::	~StationListHeader()
{
	delete[] STATIONS;
}


bool
StationListHeader	::	Verify() {
		// TODO:!
	std::string cook = COOKIE;
	return cook == "emslFunSize";
}

