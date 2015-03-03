/*
	ParseStation for Crutem 4 station files
*/

#ifndef CC_PARSE_CRUTEM4_H
#define CC_PARSE_CRUTEM4_H

#include <string>
#include <vector>

#include "StationListFormat.h"
#include "StdTypedefs.h"
#include "ParseStation.h"

namespace CRUTEM4 {

	/*
		ParseCollated
			Parses Crutem4 (4.2.0) header/data file combo often released
			to disclose stations used in the creation of publicized global
			average temperature series.
	*/

	// Does not return until complete
	std::string		ParseCollated(	const LString& headerFile,
									const LString& dataFile,
									LStationList& out	);

	std::string		ParseStation(	const LString& header,
									const LStringList& data,
									Station* output	);
			// non-empty string is error message



	std::string		ParseDirectory(	const LString& path,
									LStationList& out);

	std::string		ParseStation(	const LString& path,
									Station* out	);



);	// end namespace




#endif // CC_PARSE_CRUTEM4_H

