
/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/


/*
	CrutemConvert
		Designed to parse the station files as released with the CRUTEM4
		datasets prior to version 4.3.0.

		This format included a header file with entries, such as :
		"10010 709   87   10 Jan Mayen   NORWAY   19212011  541921    1  287
"

		And a data file which included all stations' monthly data preceded by
		the appropriate header entry.

		Future version plans:
			1.0 - Include ability to parse through the raw station files
				- Implement ability to output to emsl portable file format.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "IDAvgAccum.h"
#include "ParseArgs.h"
#include "StdTypedefs.h"
#include "StationListFormat.h"


LStringList&	Split(const std::string& str, char delim, LStringList& out) {
	using namespace std;
	stringstream strstr(str);
	string token;
	while (getline(strstr, token, delim)) {
		if (token.size() > 0)
			out.push_back(token);
	}

	return out;
}


std::string	ParseFile(const char* path, LStringList& output)
{
	using namespace std;
	printf("Parsing %s: ", path);
	fflush(stdout);

	ifstream file(path);
	if (file.fail()) {
		printf("failed! FILE ERROR\n");
		return "file error";
	}

	output.clear();		// start with an empty list
	string line;
	while (getline(file, line)) {
		if (line != "")
			output.push_back(line);
	}

	printf("complete (%li lines)\n", output.size());
	return "";
}


std::string	ParseStation(const LString& header, const LStringList& data,
						Station& output, int32& dataIndex)
{
	using namespace std;
	LStringList split;
	Split(header, ' ', split);

	LString stationID = split[0];
	for (int32 i = dataIndex; i < data.size(); ++i) {
		if (data[i].find(stationID.c_str()) != std::string::npos) {
			dataIndex = i;	// so we don't keep scanning the whole document...
			/*
				data holds the lines of the actual data, which begins with
				the station header.  We have identified that the header is ours
				and it is data[i].
				This means the next lines which begin with a year (four char #)
				is our year data.

				We need to extract, from our header, our start and end year,
				which is held as a 8 char string in index 6 of our header, except
				when the station name or nation has spaces!!

				The easy solution is to go from the right..., and back off 4
			*/
			int32 pos = split.size() - 4;
			LString tmp1 = split[pos].c_str(),
					tmp2;

			tmp2 = tmp1.substr(4, 4);
			tmp1.resize(4);

			output.STARTYEAR = atoi(tmp1.c_str());
			output.ENDYEAR = atoi(tmp2.c_str());

			int32 yearCount = output.ENDYEAR - output.STARTYEAR;
			if (yearCount > 400
				|| yearCount <= 0)
				return "Bad year count for station";

			output.ID = atoi(stationID.c_str());
			output.LAT = atof(split[1].c_str())/10.0;
			output.LON = -1 * (atof(split[2].c_str())/10.0);
			output.ELEV = atoi(split[3].c_str());


			// strings between 4 and pos are the station name and the nation
			// The formatting doesn't provide any clues, but it seems most nations
			// in the list don't contain spaces, so we'll work with that since...
			// Names don't matter anyway...
			tmp1 = split[4];

			//output.NAME = split[4];
			for (int32 j = 5; j < pos -1 ; ++j) {
				tmp1 += string(" ");
				tmp1 += split[j].c_str();
			}

			tmp1.erase(std::remove(tmp1.begin(), tmp1.end(), '-'), tmp1.end());
			int32 lastIndex = tmp1.size();
			if (lastIndex > 126)
				lastIndex = 126;

			for (int32 j = 0; j != lastIndex; ++j)
				output.NAME[j] = tmp1[j];

			output.NAME[lastIndex] = '\0';

			tmp1 = split[pos-1];
			tmp1.erase(std::remove(tmp1.begin(), tmp1.end(), '-'), tmp1.end());

			lastIndex = tmp1.size();
			if (lastIndex > 62)
				lastIndex = 62;

			for (int32 j = 0; j != lastIndex; ++j)
				output.COUNTRY[j] = tmp1[j];

			output.COUNTRY[lastIndex] = '\0';

			/*
				FINALLY!  On to the actual data!
			*/

			output.DATA = new YearData[yearCount];

			pos = 1;
			int32 yearIndex = 0;
			LStringList splitYear;
			for (int32 year = output.STARTYEAR; year < output.ENDYEAR; ++year) {
				// data[i] holds the header data
				// i + pos is the next string for the next year
				splitYear.clear();

				tmp1.clear();
				tmp1 = to_string(year);
				Split(data[i + pos], ' ', splitYear);

				if (tmp1 != splitYear[0].c_str()) {
					printf("\n\nERROR! %s != %s\n\n",
						tmp1.c_str(),
						splitYear[0].c_str());
						snooze(1300000);	// to make the error stand out!!
				} else {
					// Success!
					// Each year has the following format:

					//   0   1   2   3   4   5   6   7   8   9   10  11  12  13
					// YEAR JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC AVG
					YearData& yd = output.DATA[yearIndex];

					yd.YEAR = year;
					yd.JAN = atof(splitYear[1].c_str()) / 10.0;
					yd.FEB = atof(splitYear[2].c_str()) / 10.0;
					yd.MAR = atof(splitYear[3].c_str()) / 10.0;
					yd.APR = atof(splitYear[4].c_str()) / 10.0;
					yd.MAY = atof(splitYear[5].c_str()) / 10.0;
					yd.JUN = atof(splitYear[6].c_str()) / 10.0;
					yd.JUL = atof(splitYear[7].c_str()) / 10.0;
					yd.AUG = atof(splitYear[8].c_str()) / 10.0;
					yd.SEP = atof(splitYear[9].c_str()) / 10.0;
					yd.OCT = atof(splitYear[10].c_str()) / 10.0;
					yd.NOV = atof(splitYear[11].c_str()) / 10.0;
					yd.DEC = atof(splitYear[12].c_str()) / 10.0;
					yd.AVG = 0;

					yd.VALID = true;
				}

				++yearIndex;
				++pos;
			}

			return "";
		}// found station ID
	} // end data loop

	// fall-through is failure
	return "Unable to find data for station";
}




int main(int argc, char**argv)
{
	using namespace std;
	// parse input
	PAOutput* pa = ParseArgs(argc, argv);
	if (pa == nullptr)
		return 0;

        if (pa->returnValue != 0)
            return pa->returnValue;

	/*
		Parsing data files
	*/
	LStringList header, ignoreEntries;
	LString error = ParseFile(pa->headerFile.c_str(), header);
	if (error != "") {
		printf("ERROR: \"%s\"\n");
		return 2;
	}

	if (pa->expectIgnored || pa->autoValues) {
		error = ParseFile(pa->ignoreFile.c_str(), ignoreEntries);
		if (error != "") {
			printf("ERROR: \"%s\"\n");
			if (pa->expectIgnored)
				return 1;
		}
	}

	printf("\t%li stations in header (will ignore %li stations)\n", header.size(),
		ignoreEntries.size());

	// read data file
	LStringList data;
	error = ParseFile(pa->dataFile.c_str(), data);

	if (error != "") {
		printf("ERROR: \"%s\"\n");
		return 3;
	}

	// Create station list
	std::vector<Station*> StationList;
	printf("Searching for stations in data...\n");
	LString stationHeader;

	Station* station = NULL;
	int32 dataIndex = 0;

	bool ignore = false;
	int8 showStat = 64;

	for (int32 i = 0; i < header.size(); ++i) {
		ignore = false;
		stationHeader = header[i];
		station = new Station();
		if (stationHeader == "")
			error = "NULL stationHeader";
		else if (station == NULL)
			error = "NULL entry in StationList";
		else
		 {	// implement ignore list:

			for (const auto & s : ignoreEntries) {
				if (stationHeader.find(s) != std::string::npos) {
					ignore = true;
					break;
				}
			}

			if (ignore == false)
				error = ParseStation(stationHeader, data, *station, dataIndex);
			else
				error = "";
		 }

		if (error != "") {
			printf("\r\t\t\t\t\t\t\t\t\t\t\t\t\t");
			printf("\nERROR! Station %li not found:\n", i);
			printf("\tHeader: \"%s\"\n", stationHeader.c_str());
			printf("\tParser: \"%s\"\n", error.c_str());
		} else if (!ignore) {
			if (showStat > 64) {
				printf("\r%6li: %80s ", i, stationHeader.c_str());
				fflush(stdout);
				showStat = 0;
			}
			++showStat;
			LStringList sItems;
			Split(stationHeader, ' ', sItems);

			StationList.push_back(station);
			station = NULL;
		}
	}
	printf("\r\t\t\t\t\t\t\t\t\t\t\t\t\r");
	printf("%li stations in list\n", StationList.size());

	double accum[12];
	int32 missing[12];
	for (int32 i = 0; i < 12; ++i) {
		accum[i] = 0.0;
		missing[i] = 0;
	}

	IDAvgAccum<uint32, double>	globalAverage;

	// Calculations

	uint32 yearCount = 0, totalMissing = 0;

	#define HANDLE_MONTH(X, Y) 	if (yd.Y > -99 ){	 		\
									yd.AVG += yd.Y ; 		\
									accum[ X ] += yd.Y ;	\
								} else {					\
									missing[ X ]++;			\
									missingInYear++;		\
									totalMissing++;			\
								}

	float stationComplete = 0;
	uint64 year2000 = 0;

	for (int32 i = 0; i < StationList.size(); ++i) {
		station = StationList[i];
		totalMissing = 0;
		yearCount = station->ENDYEAR - station->STARTYEAR;
		for (int32 j = 0; j < yearCount; ++j) {
			double yearAvg = 0;
			int32 missingInYear = 0;
			YearData& yd = station->DATA[j];
			yd.AVG = 0;

			HANDLE_MONTH(0, JAN);
			HANDLE_MONTH(1, FEB);
			HANDLE_MONTH(2, MAR);
			HANDLE_MONTH(3, APR);
			HANDLE_MONTH(4, MAY);
			HANDLE_MONTH(5, JUN);
			HANDLE_MONTH(6, JUL);
			HANDLE_MONTH(7, AUG);
			HANDLE_MONTH(8, SEP);
			HANDLE_MONTH(9, OCT);
			HANDLE_MONTH(10, NOV);
			HANDLE_MONTH(11, DEC);

			yd.AVG /= (12 - missingInYear);
			if (!std::isnan(yd.AVG))
				globalAverage.add((station->STARTYEAR + j), yd.AVG);

			if ((station->STARTYEAR + j) == 2000 && !std::isnan(yd.AVG)){
				year2000 = year2000 + yd.AVG;
			}

			if (showStat > 64){
				printf("\r  Calculating: %5.2f%%  \t(%s)\t\t\t",
					stationComplete, station->COUNTRY);
				fflush(stdout);
				showStat = 0;
			}
			showStat++;

		} // end for each year
		// Calculate 'quality' of station data completeness

			station->QUALITY = 100.0 * (1.0 - ((float)totalMissing) / (yearCount * 12.0));

		// Calculate averages for each month
		for (int32 j = 0; j < 12; ++j) {
			station->AVERAGES[j] = accum[j] / (yearCount - missing[j]);
			// reset for next station
			accum[j] = 0;
			missing[j] = 0;
		}

		/*
			TODO: Insert Station into EMCoordCell
		*/



		stationComplete = 100.0 * ((double)i / (double)StationList.size());
	}
	printf("\r\t\t\t\t\t\t\t\t\t\t\t\t\r");

	/*
		Save data to file!
	*/

	switch (pa->outputTarget ) {
            case OUTPUT_TO_CSV: {
				std::ofstream outputStr(pa->outputFile.c_str());
				outputStr << "YEAR,AVERAGE,STATIONS\n";
				globalAverage.sort();
				globalAverage.for_each(
					[&outputStr](uint32 year, double average, uint32 count) {
					outputStr << year << "," << average << "," << count << "\n";
				});
				cout << "Wrote CSV data to \"" << pa->outputFile << "\"\n";
				break;
			}

            case OUTPUT_TO_EMSL:
                cerr << "EMSL output not currently implemented!\n";
                break;

            case OUTPUT_TO_CONSOLE:
				cout << "YEAR\tAVG \tCOUNT\n";
				globalAverage.sort();
				globalAverage.for_each(
					[](uint32 year, double average, uint32 count){
					cout << year << "\t" << setprecision(2) << average << "\t" << count << "\n";
				});
                break;
			
			case OUTPUT_TO_PORT:
				cerr << "Output to port currently unimplemented\n";
                break;
			
			default:
				cerr << "Confused by output target: " << pa->outputTarget;
	}

	return 0;
}

