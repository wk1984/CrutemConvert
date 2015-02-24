#include <algorithm>
#include <iostream>
#include <string>

#include "StdTypedefs.h"
#include "ParseArgs.h"


PAOutput	::	PAOutput()
	:
	headerFile	(DEFAULT_HEADERFILE),
	dataFile	(DEFAULT_DATAFILE),
	outputFile	(DEFAULT_OUTPUTFILE),
	ignoreFile	(DEFAULT_IGNOREFILE),

	expectIgnored(false),

	outputTarget(OUTPUT_TO_CONSOLE),

	useGrid		(false),
	gridSize	(5.0),

	interpolate	(false),
	interpolateDayCount(3),

	infill		(false),
	maxInfillSpan(1),

	findStation	(false),

	singleCell	(false)
	{}



void	PrintHelp()
{
		using namespace std;
		cout << "\nlooncraz(tm) CrutemConvert - " << CRUCON_VER_S << endl;
		cout << "\tConverts CRUTEM4 data (prior to 4.3.0 format change) to a CSV\n";
		cout << "\tfile containing annual average and station count OR to an emsl\n";
		cout << "\tfile which contains all data ready for use in LoonEarthModel.\n";
		cout << "\n\tYou must provide both of the following:\n";
		cout << "\t\t-h=\"path/to/headerfile.txt\"\n";
		cout << "\t\t-d=\"path/to/datafile.txt\"\n";
		cout << "\tand one of the following:\n";
		cout << "\t\t-o=\"path/to/outputfile.csv\"\n";
		cout << "\t\t-o=\"path/to/outputfile.emsl\"\n";
		cout << "\t\t\tOutput format is determined by extension.\n";
		cout << "\t\t-i=\"path/to/ignorestationlist.txt\"\n";

		cout << endl;
}


LString	StripArg(const char* argv) {
	using namespace std;
	LString result = argv;
	if (result.find("=") != std::string::npos) {
		result.erase(0, 3);	// -?=
		result.erase(remove(result.begin(), result.end(), '"'), result.end());
		return result;
	}

	return "";
}


bool IsValidArg(const char* arg)
{
	if (	arg[1] == 'h'	// header
		||	arg[1] == 'd'	// data
		||	arg[1] == 'o'	// output
		||	arg[1] == 'i'	// ignore
		)
		return arg[2] == '=';

	return false;
}


PAOutput*	ParseArgs(int argc, char** argv)
{
	PAOutput* pa = new PAOutput();

	bool useAutoValues = false;

	if (argc > 1 && LString(argv[1]) == "-auto") {
		useAutoValues = true;
		pa->expectIgnored = true;
		printf("Searching for files...\n");
	} else if (argc < 4) {
		PrintHelp();
		return nullptr;
	}

	LString 	headerFile	("data/header.txt"),
			 	dataFile	("data/data.txt"),
			 	outputFile	("data/output.csv"),
			 	ignoreFile	("data/missing.txt");

	if (!useAutoValues)
	for (int32 i = 1; i < argc; ++i) {
		if (!IsValidArg(argv[i])) {
			fprintf(stderr, "Command not understood:\n\t\"%s\"\n", argv[i]);
			PrintHelp();
			return nullptr;
		}

		switch (argv[i][1]) {
			case 'h':			// header
				pa->headerFile = StripArg(argv[i]);
				break;
			case 'd':			// data
				pa->dataFile = StripArg(argv[i]);
				break;
			case 'o':			// output
				pa->outputFile = StripArg(argv[i]);
				if (pa->outputFile.find(".emsl") != std::string::npos) {
					pa->outputTarget = OUTPUT_TO_EMSL;
					cerr << "EMSL support not implemented, check for a newer";
					cerr << " version!\n";
					return nullptr;
				}
				break;
			case 'i':			// ignore entries
				pa->ignoreFile = StripArg(argv[i]);
				pa->expectIgnored = true;
				break;
			default:
				break;
		}
	}

	return pa;
}

