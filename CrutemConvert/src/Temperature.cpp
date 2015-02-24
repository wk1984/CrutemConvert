/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/


#include "Temperature.h"

//namespace Temperature {

// #pragma mark EMTemperature


bool
EMTemperature	::	IsValid		() const
{
	return fT >= 0;	// can't be below absolute zero
}	// would have used absolute zero, but double compares suck...


double
EMTemperature	::	toKelvin	() const
{
	return fT;
}


double
EMTemperature	::	toCelsius	() const
{
	return kToc(fT);
}


double
EMTemperature	::	toFahrenheit() const
{
	return kTof(fT);
}


TScale
EMTemperature	::	Scale		() const
{
	return fScale;
}


EMTemperature&
EMTemperature::operator += (const EMTemperature& other)
{
	EMTemperature proxy = *this + other;
	fT = proxy.toKelvin();
	return *this;
}


EMTemperature&
EMTemperature::operator -= (const EMTemperature& other)
{
	EMTemperature proxy = *this - other;
	fT = proxy.toKelvin();
	return *this;
}





/*
	Run-time executions
	Inter-EMTemperature calculations
*/

EMTemperature operator + (const EMTemperature& t1, const EMTemperature& t2)
{
	double diff = 0.0;

	switch (t2.Scale()) {	// we only care about the rhs scale
		case TScale::Kelvin:
			diff = t2.toKelvin();
			break;
		case TScale::Celsius:	// same scaling, so very simple
			diff = t2.toCelsius()
			;break;
		case TScale::Fahrenheit:
			// convert K to F
			double inF = t1.toFahrenheit() + t2.toFahrenheit();
			return EMTemperature(fTok(inF), TScale::Kelvin);
	}

	return EMTemperature(t1.toKelvin() + diff,
		TScale::Kelvin);
}


EMTemperature operator - (const EMTemperature& t1, const EMTemperature& t2)
{

	double diff = 0.0;

	switch (t2.Scale()) {	// we only care about the rhs scale
		case TScale::Kelvin:
			diff = t2.toKelvin();
			break;
		case TScale::Celsius:	// same scaling, so very simple
			diff = t2.toCelsius()
			;break;
		case TScale::Fahrenheit:
			// convert K to F
			double inF = t1.toFahrenheit() - t2.toFahrenheit();
			return EMTemperature(fTok(inF), TScale::Kelvin);
	}

	return EMTemperature(t1.toKelvin() - diff,
		TScale::Kelvin);
}


bool operator == (const EMTemperature& t1, const EMTemperature& t2)
{
	// convert to float for test due to double rounding errors
	return ((float)t1.toKelvin()) == ((float)t2.toKelvin());
}

bool operator != (const EMTemperature& t1, const EMTemperature& t2)
{
	// convert to float for test due to double rounding errors
	return ((float)t1.toKelvin()) != ((float)t2.toKelvin());
}



