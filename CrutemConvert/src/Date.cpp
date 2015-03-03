/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/

#include <cstdint>
#include <stdio.h>
#include "StdTypedefs.h"

#include "Date.h"


int64
EMDate	::	Year		() const
{
		return fYear;
}


int16
EMDate	::	Month		() const
{
	return fMonth;
}


int16
EMDate	::	Day			() const
{
	return fDay;
}


bool
EMDate	::	IsYearValid	() const
{
	return fYear < INT64_MAX;
}


bool
EMDate	::	IsMonthValid() const
{
	return fMonth > 0 && fMonth < 13;
}


bool
EMDate	::	IsDayValid() const
{
	int maxDays = DaysInMonth();
	if (maxDays == 0) maxDays = 31;

	return fDay > 0 && fDay < maxDays + 1;
}



bool
EMDate	::	IsValid	() const
{	// only one needs to be valid
	return IsYearValid()
		||	IsMonthValid()
		||	IsDayValid();
}


bool
EMDate	::	IsKeepDaySet() const
{
	return fKeepDay > 0 && fKeepDay < 32;
}


int16
EMDate	::	KeepDay		() const
{
	return fKeepDay;
}


void
EMDate	::	SetKeepDay	(int16 day)
{
	fKeepDay = day;
	_enforceKeepDay();
}



bool
EMDate	::	IsLeapYear	(int64 year) const
{
	if (year == INT64_MAX) {
		if (!IsYearValid())	return false;
		year = fYear;
	}

	if ((year % 4) != 0)	return false;
	if ((year % 100) != 0)	return true;
	if ((year % 400) != 0)	return false;

	return true;
}


int16
EMDate	::	DaysInMonth	(int16 month) const
{
	if (month < 1 || month > 12) {
		if (!IsMonthValid())	return 0;
		month = fMonth;
	}

	switch (month) {
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
	}

	if (month == 2)
		return IsLeapYear() ? 29 : 28;

	return 31;
}


int64
EMDate	::	DaysFrom	(EMDate date) const
{	// due to our large date range with pre-big bang reference point
	// we can't use the easy ways :-(
	// This method isn't 100% accurate, sometimes being off by a day,
	// but this is good enough for my needs, for now.

	EMDate* first = nullptr, *second = first;

	if (*this > date) {
		first = &date;
		second = const_cast<EMDate*>(this);
	} else if (*this < date) {
		first = const_cast<EMDate*>(this);
		second = &date;
	} else
		return 0;

	// brute force method (sadly, and I'm lazy, I just need it to work)
	EMDate temp = *first;

	int64 lastYear = 0, lastMonth = 0, days = 0;

	while (temp < *second) {
		temp += 1_year;
		lastYear = temp.IsLeapYear() ? 366 : 365;
		days += lastYear;
	}
	days -= lastYear;
	temp -= 1_year;

	while (temp < *second) {
		temp += 1_month;
		lastMonth = temp.DaysInMonth();
		days += lastMonth;
	}
	days -= lastMonth;
	temp -= 1_month;

	while (temp < *second) {
		temp += 1_day;
		++days;
	}

	while (temp > *second) {
		temp -= 1_day;
		--days;
	}

	return days;
}


void
EMDate	::	PrintToStream(const char* pfx, const char* sfx) const
{
	std::string monName;
	switch (fMonth) {
		case 1:	monName = "Jan"; break;
		case 2:	monName = "Feb"; break;
		case 3:	monName = "Mar"; break;
		case 4:	monName = "Apr"; break;
		case 5:	monName = "May"; break;
		case 6:	monName = "Jun"; break;
		case 7:	monName = "Jul"; break;
		case 8:	monName = "Aug"; break;
		case 9:	monName = "Sep"; break;
		case 10:monName = "Oct"; break;
		case 11:monName = "Nov"; break;
		case 12:monName = "Dec"; break;
		default:monName = "---"; break;
	}

	printf("%s%s/%02i/%lli%s", pfx, monName.c_str(), fDay, fYear, sfx);
}


EMDate&
EMDate	::	operator << (const EMDate& other)
{
	if (other.IsYearValid())	this->fYear = other.Year();
	if (other.IsMonthValid())	this->fMonth = other.Month();
	if (other.IsDayValid())		this->fDay = other.Day();
	_enforceKeepDay();
	return *this;
}


EMDate&
EMDate	::	operator += (const EMDate& other)
{
		if (!other.IsValid())	// we don't care if we are valid!
		return *this;

	if (other.fDay > 0) {
		if (IsDayValid() == false) {
			fDay = other.fDay;
			_rollDay();
		}
		else {
			fDay += other.fDay;
			_rollDay();
			_rollMonth();
		}
	}

	if (other.IsMonthValid()) {
		if (IsMonthValid() == false) {
			fMonth = other.fMonth;
			_rollMonth();
		}
		else {
			int16 daysInPrevMonth = DaysInMonth();
			fMonth += other.fMonth;
			_rollMonth();

			if (fDay == daysInPrevMonth)
				fDay = DaysInMonth();

			if (fDay > DaysInMonth())
				fDay = DaysInMonth();
		}
	}

	if (other.IsYearValid()) {
		if (IsYearValid() == false)
			fYear = other.fYear;
		else
			fYear += other.fYear;
	}
	_enforceKeepDay();

	return *this;
}


EMDate&
EMDate	::	operator -= (const EMDate& other)
{
	if (!IsValid() || !other.IsValid())
		return *this;	// you can't subtract from an invalid date

	if (other.fDay > 0) {
		if (IsDayValid()) {
			fDay -= other.fDay;
			_rollDay();
			_rollMonth();
		}
	}

	if (other.IsMonthValid()) {
		if (IsMonthValid()) {
			int16 daysInPrevMonth = DaysInMonth();
			fMonth -= other.fMonth;
			_rollMonth();

			if (fDay == daysInPrevMonth)
				fDay = DaysInMonth();

			if (fDay > DaysInMonth())
				fDay = DaysInMonth();
		}
	}

	if (other.IsYearValid()) {
		if (IsYearValid())
			fYear -= other.fYear;
	}
	_enforceKeepDay();
	return *this;
}



//#pragma mark private


void
EMDate	::	_rollDay()
{
	int16 daysInMonth = DaysInMonth();
	if (daysInMonth < 1) {
		fMonth = 1;
		daysInMonth = DaysInMonth();
	}

	//printf("\t\t_rollDay() : %i / %i\n", fDay, daysInMonth);

	if (fDay < 1) {	// going backwards!
		--fMonth;
		_rollMonth();
		fDay += DaysInMonth();
	} else if (fDay > daysInMonth) {
		++fMonth;

		fDay = daysInMonth - fDay;
		daysInMonth = DaysInMonth();

		if (fDay > daysInMonth)
			fDay = daysInMonth;

		if (fDay < 1)	// result of fDay > daysInMonth
			fDay = -fDay;

		_rollMonth();
	}

	if (!IsDayValid())
		_rollDay();
		// should never recurse more than once
}


void
EMDate	::	_rollMonth()
{
	if (fMonth < 1) {
		--fYear;
		fMonth += 12;
	} else if (fMonth > 12) {
		++fYear;
		fMonth -= 12;
	}
}


bool
EMDate	::	_enforceKeepDay()
{
	if (fKeepDay < 1 || fKeepDay > 31)
		return false;

	if (fDay != fKeepDay) {
		fDay = fKeepDay;

		int16 daysInMonth = DaysInMonth();
		if (fDay > daysInMonth)
			fDay = daysInMonth;

		return true;
	}
	return false;
}


// #pragma mark external operators

EMDate	operator + (const EMDate& d1, const EMDate& d2)
{
	EMDate val = d1;	val += d2;
	return val;
}


EMDate	operator - (const EMDate& d1, const EMDate& d2)
{
	EMDate val = d1;	val -= d2;
	return val;
}


bool	operator == (const EMDate& d1, const EMDate& d2)
{
	return	d1.IsValid()
		&&	d2.IsValid()
		&&	d1.Year() == d2.Year()
		&&	d1.Month() == d2.Month()
		&&	d1.Day() == d2.Day();
}


bool operator != (const EMDate& d1, const EMDate& d2)
{
	return !(d1 == d2);
}


bool	operator > (const EMDate& d1, const EMDate& d2)
{	// brutally rolled out for a reason, leave it be... I know, I know, just leave it...
	if (d1.IsYearValid()) {
		if (d2.IsYearValid() == false)
			return true;	// valid year wins!

		if (d1.Year() > d2.Year())
			return true;
		else if (d1.Year() < d2.Year())
			return false;
	}

	if (d1.IsMonthValid()) {
		if (d2.IsMonthValid() == false)
			return true; // valid month wins!

		if (d1.Month() > d2.Month())
			return true;
		else if (d1.Month() < d2.Month())
			return false;
	}

	if (d1.IsDayValid()) {
		if (d2.IsDayValid() == false)
			return true;// valid day ... does... something...

		if (d1.Day() > d2.Day())
			return true;
		else if (d1.Day() < d2.Day())
			return false;
	}

	return false;
}


bool	operator < (const EMDate& d1, const EMDate& d2)
{
	if (d1 > d2)	return false;
	return d1 != d2 ;
}



//#pragma mark EMDateRange

EMDateRange	::	EMDateRange	(int)
	:
	fDayCount	(-1),
	fMonthCount	(-1),
	fYearCount	(-1)
	{}


EMDateRange	::	EMDateRange	(const EMDate& d1, const EMDate& d2)
	:
	fDayCount	(-1),
	fMonthCount	(-1),
	fYearCount	(-1)
{
	SetTo(d1, d2);
}


EMDateRange	::	EMDateRange	(const EMDateRange& drange)
	:
	fDayCount	(-1),
	fMonthCount	(-1),
	fYearCount	(-1)
{
	fFirst = drange.fFirst;
	fLast = drange.fLast;
	fDayCount = drange.fDayCount;
	fMonthCount = drange.fMonthCount;
	fYearCount = drange.fYearCount;
}


EMDateRange	::	~EMDateRange()
{
}


int64
EMDateRange	::	CountDays	() const
{
	if (fDayCount >= 0 || IsValid() == false)
		return fDayCount;

	fDayCount = fFirst.DaysFrom(fLast);
	return fDayCount;
}


int64
EMDateRange	::	CountMonths	() const
{
	if (fMonthCount >= 0 || !IsValid())
		return fMonthCount;

	fMonthCount = 0;
	foreach_month([this](const EMDate&) {
			++fMonthCount;
		});

	return fMonthCount;
}


int64
EMDateRange	::	CountYears	() const
{
	if (fYearCount >= 0 || !IsValid())
		return fYearCount;

	// This is the easiest method, and we may already
	// have the day count cached, otherwise we do now ;-)
	fYearCount =  ((double)CountDays())/ 365.25;

	return fYearCount;
}


bool
EMDateRange	::	IsValid() const
{
	return fFirst.IsValid() && fLast.IsValid();
}


void
EMDateRange	::	foreach_day	(function<void(const EMDate&)> func) const
{
	EMDate date = fFirst;

	while (date < fLast) {
		func(date);
		date += 1_day;
	}
}


void
EMDateRange	::	foreach_month(function<void(const EMDate&)> func) const
{
	EMDate date = fFirst;

	while (date < fLast) {
		func(date);
		date += 1_month;
	}
}


void
EMDateRange	::	foreach_year(function<void(const EMDate&)> func) const
{
	EMDate date = fFirst;

	while (date < fLast) {
		func(date);
		date += 1_year;
	}
}


const EMDate&
EMDateRange	::	First	() const
{
	return fFirst;
}


const EMDate&
EMDateRange	::	Last	() const
{
	return fLast;
}


void
EMDateRange	::	SetTo	(const EMDate& d1, const EMDate& d2)
{
	_Reset();
	if (d2 > d1) {
		fLast = d1;
		fFirst = d2;
	} else {
		fFirst = d1;
		fLast = d2;
	}
}


void
EMDateRange	::	Include	(const EMDate& date)
{
	if (date > fLast) {
		fLast = date;
		_Reset();
	} else if (date < fFirst) {
		fFirst = date;
		_Reset();
	}
}


void
EMDateRange	::	_Reset() const
{
	fDayCount = -1;
	fMonthCount = -1;
	fYearCount = -1;
}


