#include "Build.h"

#include "StationCalc.h"



namespace Calc	{


Result	::	Result	()
	{
		for (auto& d : fFinal)
			d = FAILURE;
	}


Result	::	~Result	(){}


double
Result	::	Get		(const EMDate& date) const
{
	// ignore day!

	if (	date.IsYearValid() == false
		&&	date.IsMonthValid() == false	)
		return FAILURE;

	if (	date < fDRange.First()
		||	date > fDRange.Last()	)
		return FAILURE;

	double result = FAILURE;

	if (date.IsMonthValid()) {
		if (date.IsYearValid()) {
			for (auto& p : fData) {
				if (p.first == date.Year()) {
					result = p.second[date.Month() - 1];
					break;
				}
			}
		} else	// month only, which means fFinal only
			result = fFinal[date.Month() - 1];
	} else {	// year only
		for (auto& p : fData) {
			if (p.first == date.Year()) {
				result = p.second[12];
				break;
			}
		}
	}

	return result;
}


void
Result	::	Set		(const EMDate& date, double val)
{
	if (	date.IsYearValid() == false
		||	date.IsMonthValid() == false	)
		return;

	fDRange.Include(date);

	for (auto&p : fData) {
		if (p.first == date.Year()) {
			p.second[date.Month()-1] = val;
			return;
		}
	}

	// Don't have the year yet, create it:

	fData.emplace_back();

	auto& p = fData.back();
	p.first = date.Year();
	p.second[date.Month()-1] = val;
}



void
Result	::	SetFinal(double* final)
{
	if (final == nullptr)	return;	// do something more drastic?

	for (int8 i = 0; i < 13; ++i)
		fFinal[i] = final[i];
}


const double*
Result	::	GetFinal() const
{
	return fFinal;
}



const	EMDateRange&
Result	::	DateRange	() const
{
	return fDRange;
}



	// iteration

void
Result	::	foreach_final
	(std::function<void(int8, double&)> func)
{
	for (int8 i = 0; i < 13; ++i)
		func(i, fFinal[i]);
}



void
Result	::	foreach_final
	(std::function<void(int8, double)> func) const
{
	for (int8 i = 0; i < 13; ++i)
		func(i, fFinal[i]);
}



// foreach_[TimePeriod]
void
Result	::	foreach_day
	(std::function<void(const EMDate&, double&)> func)
{
		/*		EMPTY FOR NOW	*/
	/*		Requires interpolation		*/
}



void
Result	::	foreach_month
	(std::function<void(const EMDate&, double&)> func)
{
	EMDate date;
	for (auto& p : fData) {
		date << _year(p.first);
		for (int8 i = 0; i < 12; ++i) {
			date << _month(i);
			func(date, p.second[i]);
		}
	}
}



void
Result	::	foreach_year
	(std::function<void(uint32, double&)> func)
{
	for (auto& p : fData)
		func(p.first, p.second[12]);
}




// const foreach
void
Result	::	foreach_day
	(std::function<void(const EMDate&, double)> func) const
{

}



void
Result	::	foreach_month
	(std::function<void(const EMDate&, double)> func) const
{
	EMDate date;
	for (auto& p : fData) {
		date << _year(p.first);
		for (int8 i = 0; i < 12; ++i) {
			date << _month(i);
			func(date, p.second[i]);
		}
	}
}



void
Result	::	foreach_year
	(std::function<void(uint32, double)> func) const
{
	for (auto& p : fData)
		func(p.first, p.second[12]);
}


//#pragma mark Single Station

string	IsValid	(const Station* station, EMDateRange& range)
{
	if (station == nullptr)	return "NULL station";

	return "";
}


string	Averages	(const Station* station, Result& out, EMDateRange dRange)
{
	string error = IsValid(station, dRange);
	if (error != "")	return SERROR(error);
}


string	Deviations	(const Station* station, Result& out, EMDateRange dRange)
{

}


string	Infill		(const Station* station, Result& out, EMDateRange dRange)
{

}


string	Trend		(const Station* station, Result& out, EMDateRange dRange)
{

}


string	Variance	(const Station* station, Result& out, EMDateRange dRange)
{

}





}; // end namespace

