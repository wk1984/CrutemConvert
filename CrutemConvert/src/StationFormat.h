/*
	Replcement for archaic StationListFormat system.
*/
#ifndef EM_PORTABLE_STATION_FORMAT_H
#define EM_PORTABLE_STATION_FORMAT_H

#include <future>
#include <string>
#include <vector>

#include "FS.h"

using namespace std;

namespace emp {	// Earth Model Portable


enum Format {
	UNKNOWN,
	CRUTEM4_COLLATED,
	CRUTEM4_DIRECTORY,
	StationList,
	Station,
	NETCDF,
	CSV
};

class YearData {
public:
	int32 				YEAR;
	float				JAN, FEB, MAR, APR, MAY, JUN,
						JUL, AUG, SEP, OCT, NOV, DEC;
	YearData();

		float&	operator [] (int at){
			switch (at) {
				case 0:	return JAN;		case 1: return FEB;
				case 2: return MAR;		case 3: return APR;
				case 4: return MAY;		case 5: return JUN;
				case 6: return JUL;		case 7: return AUG;
				case 8: return SEP;		case 9: return OCT;
				case 10:return NOV;		case 11:return DEC;
				default:
					ERROR("Access YearData() out of range!");
			}
			return BADVAL;
		}

	const float& operator[] (int);

	float	BADVAL;
};



class Station	{
public:
							Station	();
							Station	(fs::Entry, Format);
							Station	(const string&, Format);
							Station	(const string&, const vector<string>&,
										 Format);

							~Station();

			bool			IsValid() const;

	const	YearData*		GetYear	(uint32)	const;

			void			foreach(function<void(YearData&)>);
			void			foreach(function<void(const YearData&)>) const;

	const 	EMDateRange&	DateRange() const;


			uint32			ID			() const;
			EMCoordinate	Coordinates	() const;
			int32			Elevation	() const;
	const string&			Name		() const;
	const string&			Country		() const;
			float			Quality		() const;

	// Setters
			void			Include		(const YearData&);
			void			SetID		(uint32);
			void			SetCoordinates(EMCoordinate);
			void			SetElevation(int32);
			void			SetName		(const string&);
			void			SetCountry	(const string&);
			void			SetQuality	(float);

private:
			uint32			fID;
			EMCoordinate	fCoordinates;
			int32			fElevation;
			string			fName;
			string			fCountry;
			float			fQuality;

			vector<YearData>fData;
};


/*
	Modifications increment the revision counter.
*/
class StationList {
public:
								StationList		();
								StationList		(const fs::Entry&, Format);
								StationList		(const string&, const string&);

								~StationList	();

	const	string&				InitError	() const;
			void				MakeEmpty	();

// Metadata
	const	string&				Source	() const;
	const	string&				Author	() const;
			int					MajorVersion	() const;
			int					MinorVersion	() const;
			int					Revision		() const;

			void				Add		(const Station&);
			void				Remove	(const Station&);

			void				Include	(const EMRect&);
			void				Exclude (const EMRect&);

			int					Count	() const;

			Station*			Find	(uint32);
			Station*			Find	(const string&);

	const	Station*			Find	(uint32) const;
	const	Station*			Find	(const string&) const;

			void				foreach	(function<void(Station&)>);
			void				foreach	(function<void(const Station&)>) const;

			string				Save	(const fs::Entry&, Format);
private:
			string				_Init(const fs::Entry&, Format);
			string				_Init(const string&, const string&);

			string				fInitError;

			string				fSource;
			string				fAuthor;
			int					fMajorVersion;
			int					fMinorVersion;
			int					fRevision;

	vector<Station>				fStations;
};



};	// namespace emp



#endif // EM_PORTABLE_STATION_FORMAT_H
