#ifndef L_STATION_LIST_FILE_FORMAT_H
#define L_STATION_LIST_FILE_FORMAT_H

#include <future>
#include <string>

using namespace std;

#include "StdTypedefs.h"
#include "Temperature.h"
#include "Date.h"

/*
	These represent an intermediate step before the complete
	LoonEarthModel system is up and running.

	The data is the most complete to be expected from the source data.
	LoonEarthModel stationlist file format will not be based upon this format,
	but will use EMStore and EMStorageItem and be highly dynamic and include
	security mechanisms.

	LEM will, however, be able to read this format and convert it to its
	own, very complex, non-deterministic format, so we have to keep that
	in mind here.
*/

struct YearData {
	bool				VALID;
	int32 				YEAR;
	float				JAN,
						FEB,
						MAR,
						APR,
						MAY,
						JUN,
						JUL,
						AUG,
						SEP,
						OCT,
						NOV,
						DEC,
						AVG;
	YearData();
};


class Station {
public:
	uint32 				ID;
	float 				LAT;
	float 				LON;
	int32				ELEV;
	char				NAME[128];
	char				COUNTRY[64];
	int32				STARTYEAR;
	int32				ENDYEAR;
	float				AVERAGES[12];	// monthly
	float				QUALITY;		// MONTHSGOOD:MONTHSBAD

	YearData*			DATA;

								Station();
	virtual						~Station();

			void				PrintToStream() const;

			EMDate				StartYear	() const;
			EMDate				EndYear		() const;

			EMTemperature		AverageFor	(EMDate, bool ignoreDay = true);
			EMTemperature		AverageFor	(EMDate, EMDate, bool = true);

			void				for_each	(function<void(YearData&)>);
};


struct StationListHeader {
	char			COOKIE[12]; // "emslFunSize\0"
	char			SOURCE_NAME[128];
	char			SOURCE_VERS[64];
	char			SOURCE_BUILDER[128];

	uint32			DATA_FORMAT_VERSION;
	uint32			STATION_COUNT;

	char			_RESRV1[4096];

	Station*		STATIONS;

								StationListHeader();
								~StationListHeader();

			bool				Verify();
};




#endif // L_STATION_LIST_FILE_FORMAT_H

