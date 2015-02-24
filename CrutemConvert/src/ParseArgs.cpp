#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "StdTypedefs.h"
#include "ParseArgs.h"

using namespace std;


vector<pair<LString, LString>>  ParameterHelp = {
    make_pair("auto", "Automatically chooses local data files."),
    make_pair("header", "Set location of station list header file."),
    make_pair("data", "Set location of station list data file."),
    make_pair("ignore", "Set location of list of stations to ignore."),
    make_pair("output", "Set output location:\n"
                        "\t\t\t\tport:id  - local application port (Haiku only)\n"
                        "\t\t\t\tfile.csv - Comma Separated Values\n"
                        "\t\t\t\tfile.emsl- EarthModel StationList format"),
    make_pair("gridsize", "Set size of grids for use with area weighting."),
    make_pair("interpolate", "Use data interpolation to estimate daily values\n"
                            "\t\t\t\tTakes a parameter of days (default is 1)"),
    make_pair("infill", "Attempt to infill missing station data\n"
                        "\t\t\t\tTakes a parameter for maximum infill span"),
    make_pair("station", "Set search string to find a specific station"),
    make_pair("cellrect", "Limit analysis to specific cooridnate area.\n"
                            "\t\t\t\t-cellrect=\"west, north, east, south\"")
};



PAOutput	::	PAOutput()
	:
	headerFile	(DEFAULT_HEADERFILE),
	dataFile	(DEFAULT_DATAFILE),
	outputFile	(DEFAULT_OUTPUTFILE),
	ignoreFile	(DEFAULT_IGNOREFILE),

	expectIgnored   (false),

	outputTarget    (OUTPUT_TO_CONSOLE),

	useGrid		(false),
	gridSize	(5.0),

	interpolate	(false),
	interpolateDayCount(3),

	infill		(false),
	maxInfillSpan(1),

	findStation	(false),

	singleCell	(false),
        returnValue     (0)
	{}



void	PrintHelp()
{
		using namespace std;
		cout << "\nlooncraz(tm) CrutemConvert - " << CRUCON_VER_S << endl;
		cout << "\tConverts CRUTEM4 data (prior to 4.3.0 format change) to a CSV\n";
		cout << "\tfile containing annual average and station count OR to an emsl\n";
		cout << "\tfile which contains all data ready for use in LoonEarthModel.\n";
		cout << "\n\tParameters:\n";

                for (auto p : ParameterHelp) {
                    cout << "\t" << setw(16) << left << p.first << " - ";
                    cout << p.second << endl;
                }
                
		cout << endl;
}


pair<LString, LString>	SplitParam(const LString& string) {
	using namespace std;
	
        LString param = string, value;
        size_t argSep = string.find("=");

	if (argSep != std::string::npos) {
            value = string;
            value.erase(0, argSep);
            value.erase(remove(value.begin(), value.end(), '"'), value.end());

            param.resize(argSep);
        }
		param.erase(0,1);
        // make param lowercase
        transform(param.begin(), param.end(), param.begin(), ::tolower);
	return make_pair(param, value);
}



/*
    I know I could make this all pretty and stuff, but I just don't care.
 */

PAOutput*	ParseArgs(int argc, char** argv)
{
    if (argc < 2) {
        PrintHelp();
        return nullptr;
    }

    PAOutput* pa = new PAOutput();

    LStringList paramList;
    for (int i = 1; i < argc; ++i)
        paramList.push_back(argv[i]);

    for (auto param : paramList) {
        auto entry = SplitParam(param);
        printf("PARAM: %s\n", entry.first.c_str());
        if (entry.first == "auto") {
            pa->autoValues = true;
			printf("auto!\n");
		}
        else if (entry.first == "header")
            pa->headerFile = entry.second;
        else if (entry.first == "data")
            pa->dataFile = entry.second;
        else if (entry.first == "ignore") {
            pa->ignoreFile = entry.second;
            pa->expectIgnored = true;
        } else if (entry.first == "output") {
            pa->outputFile == entry.second;
            if (entry.second.find(".csv"))
                pa->outputTarget = OUTPUT_TO_CSV;
            else if (entry.second.find(".emsl"))
                pa->outputTarget = OUTPUT_TO_EMSL;
            else if (entry.second.find("port:"))
                pa->outputTarget = OUTPUT_TO_PORT;
        } else if (entry.first == "gridsize") {
            pa->useGrid = true;
            pa->gridSize = atof(entry.second.c_str());
        } else if (entry.first == "interpolate") {
            pa->interpolate = true;
            pa->interpolateDayCount = atof(entry.second.c_str());
        } else if (entry.first == "infill"){
            pa->infill = true;
            pa->maxInfillSpan = atof(entry.second.c_str());
        } else if (entry.first == "station") {
            pa->findStation = true;
            pa->findStationString = entry.second;
        } else if (entry.first == "cellrect"){
            pa->singleCell = true;
            
            istringstream ss(entry.second);
            bool w = false, n = w, e = w, s = w;
            LString tmp;
            while (getline(ss, tmp, ',')) {
                if (!w) {
                    pa->cellRect.West = atof(tmp.c_str());
                    w = true;
                } else if (!n) {
                    pa->cellRect.North = atof(tmp.c_str());
                    n = true;
                } else if (!e) {
                    pa->cellRect.East = atof(tmp.c_str());
                    e = true;                    
                } else if (!s){
                    pa->cellRect.South = atof(tmp.c_str());
                    s = true;
                } else
                    cerr << "Unknown cellrect boundary: " << tmp << endl;
            }
        } 
            // HELP 'SYSTEM'
        else {
            PrintHelp();
            return nullptr;
        }
    } // param perusal
        
    
    
    return pa;
}

