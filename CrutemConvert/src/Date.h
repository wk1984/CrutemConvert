/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/

#ifndef EM_TIME_SYSTEM_H
#define EM_TIME_SYSTEM_H


#include <cstdint>
#include <future>

using namespace std;

#include "StdTypedefs.h"

/*
	EMDate is a super-super-super simple holder for a date.

	Usage:
	EMDate date = 1999_year << 12_month << 25_day;

	That will become Dec 25, 1999.

	Restrictions:
		Days > 0 < 32
		Months > 0 < 13
		Years != 9,223,372,036,854,775,807
		No math operators
		One+ valid field(s) needed for comparison.

	You do not need to set every element, setting just the day, month, or year
	is the intended usage. For simplicity, use Is[Year/Month/Day]Valid()

	EMDate date = 1999_year;  // perfectly valid

	EMDate date;  //	Initial state is invalid
	date << 1_day;	// now valid, with Day 1 set, no year, no month

	EMDate date; // invalid
	EMDate date2; // also invalid

	if (date == date2) {
		// will never succeed
	}

	date = 1_day; // valid
	date += 1_day; // date increments

	We do not internally convert to another format, we use year, month, and day
	and roll excess values (32 days in September results in Oct 1st).

	Adding a year will increment to the next year, same date. Increment a month
	and you end up with the same day in the next month... but, there's an issue:

	What if you have day 31 in January and you add a month, so we are at Feb 30
	which is invalid, but if you add a month to Jan 31 and get a date in March
	you will not be happy, so we select Feb 28/29.

	But, if adding another month to the same date gets us Mar 28/29 you won't be
	happy either. So we give you Mar 31.

	Of course, this means that if January has day 29, 30, or 31, Febuary will
	be set to its last day, and each month thereafter will be set to its last
	day.  So, if you want a specific day, you will need to set the keep day
	flag:

		EMDate date(1999, 12, 29);
		date.SetKeepDay(29);

		Without keep day:			With keep day:
		Dec 29, 1999				Dec 29, 1999
		Jan 29, 2000				Jan 29, 2000
		Feb 29, 2000				Feb 29, 2000	// leap year, 28 otherwise
		Mar 31, 2000				Mar 29, 2000
		Apr 30, 2000				Apr 29, 2000

		Adding days to a date with KeepDay set is potentially troublesome:

		Add 31 days:
		Dec 29, 1999
		Jan 29, 2000
		Mar 29, 2000
		Apr 29, 2000
		May 29, 2000

		What happened to Feb?

		If the keep day was 30 or 31, the results get even more crazy!
		As such, you should just use months and years to change the date
		when you are trying to keep the same day of the month.
*/


class	EMDate {
public:
	explicit constexpr			EMDate();
	explicit constexpr			EMDate(int64, int16 = 0, int16 = 0);

				int64			Year		() const;
				int16			Month		() const;
				int16			Day			() const;

				bool			IsYearValid	() const;
				bool			IsMonthValid() const;
				bool			IsDayValid	() const;
				bool			IsValid		() const;

				bool			IsKeepDaySet() const;
				int16			KeepDay		() const;
				void			SetKeepDay	(int16);

				bool			IsLeapYear	(int64 = INT32_MAX) const;
				int16			DaysInMonth	(int16 = 0) const;

				int64			DaysFrom	(EMDate) const;

				void			PrintToStream(	const char* = "",
												const char* = "\n") const;

		EMDate&	operator << (const EMDate&);

		EMDate&	operator += (const EMDate& other);
		EMDate&	operator -= (const EMDate& other);

private:
				void			_rollDay();
				void			_rollMonth();
				bool			_enforceKeepDay();

				int64			fYear;

				int16			fMonth,
								fDay,
								fKeepDay;
};


// EMDateRange will automatically sort the dates by which came earliest.
// First() will always be sooner than Last()
class	EMDateRange {
public:
								EMDateRange	(int = 0);
								EMDateRange	(const EMDate&, const EMDate&);
								EMDateRange	(const EMDateRange&);

	virtual						~EMDateRange();

				int64			CountDays	() const;
				int64			CountMonths	() const;
				int64			CountYears	() const;

				bool			IsValid		() const;

				void			foreach_day	(function<void(const EMDate&)>) const;
				void			foreach_month(function<void(const EMDate&)>) const;
				void			foreach_year(function<void(const EMDate&)>) const;

		const	EMDate&			First	() const;
		const	EMDate&			Last	() const;

				void			SetTo	(const EMDate&, const EMDate&);
				void			Include	(const EMDate&);

private:
				void			_Reset() const;
				EMDate			fFirst;
				EMDate			fLast;

	mutable		int64			fDayCount,
								fMonthCount,
								fYearCount;
};


constexpr EMDate _year(int64 year)
{
	return EMDate(year);
}


constexpr EMDate _month(int16 month)
{
	return EMDate(INT64_MAX, month);
}


constexpr EMDate _day(int16 day)
{
	return EMDate(INT64_MAX, 0, day);
}


constexpr EMDate operator "" _year (unsigned long long int year)
{
	return _year(year);
}


constexpr EMDate operator "" _month (unsigned long long int month)
{
	return _month(month);
}


constexpr EMDate operator "" _day (unsigned long long int day)
{
	return _day(day);
}


EMDate	operator + (const EMDate&, const EMDate&);
EMDate	operator - (const EMDate&, const EMDate&);
bool	operator == (const EMDate&, const EMDate&);
bool	operator != (const EMDate&, const EMDate&);
bool	operator > (const EMDate&, const EMDate&);
bool	operator < (const EMDate&, const EMDate&);


// Constructors

constexpr	EMDate	::	EMDate()
	:
	fYear(INT32_MAX),
	fMonth(0),
	fDay(0),
	fKeepDay(0)
	{}

constexpr	EMDate	::	EMDate(int64 y, int16 m, int16 d)
	:
	fYear(y),
	fMonth(m),
	fDay(d),
	fKeepDay(0)
	{
	}


#endif //EM_TIME_SYSTEM_H

