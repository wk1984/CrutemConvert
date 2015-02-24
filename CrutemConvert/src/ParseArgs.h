/*
	This is NOT a generic-use header!
	For CrutemConvert only!

	Or you can spend a /lot/ of time refactoring it for your
	uses...
*/

#ifndef CC_PARSE_ARGS_H
#define CC_PARSE_ARGS_H


#define	CRUCON_VER_F	0.42
#define	CRUCON_VER_S	"0.4b"

#include "EarthCoordSystem.h"
#include <string>
using namespace std;

#	define	DEFAULT_HEADERFILE	"data/header.txt"
#	define	DEFAULT_DATAFILE	"data/data.txt"
#	define	DEFAULT_OUTPUTFILE	"data/output.txt"
#	define	DEFAULT_IGNOREFILE	"data/missing.txt"


enum OutputTo {
	OUTPUT_TO_CSV = 0,
	OUTPUT_TO_EMSL,
	OUTPUT_TO_CONSOLE,
	OUTPUT_TO_PORT
};


struct PAOutput {
	string		headerFile,
				dataFile,
				outputFile,
				ignoreFile;

	bool		expectIgnored;

	OutputTo	outputTarget;

	bool		useGrid;
	float		gridSize;

	bool		interpolate;
	float		interpolateDayCount;

	bool		infill;
	float		maxInfillSpan;

	bool		findStation;
	string		findStationString;

	bool		singleCell;	// amoeba
	EMCoordRect	cellRect;

	//
								PAOutput();
};


PAOutput*	ParseArgs	(int, char**);




#endif // CC_PARSE_ARGS_H

