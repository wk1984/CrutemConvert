/*
		(C) Copyright 2015, looncraz(tm)

		Permission is hereby granted to anyone coming across this code to use
		however they shall please provided the above copyright notice remains
		intact and unmodified.
*/

#ifndef EM_TEMPERATURE_H
#define EM_TEMPERATURE_H


//namespace Temperature {


// Scale enumerations
enum TScale {
	Kelvin,
	Celsius,
	Fahrenheit
};


// Temperature stored in kelvin
class EMTemperature {
public:
	explicit	constexpr	EMTemperature();
	explicit	constexpr	EMTemperature(double temp, TScale);

			bool				IsValid		() const;

			double				toKelvin	() const;
			double				toCelsius	() const;
			double				toFahrenheit() const;
			TScale				Scale		() const;


	EMTemperature& operator += (const EMTemperature& other);
	EMTemperature& operator -= (const EMTemperature& other);

	// if you need other maths... I'm not sure why, LOL!

private:
			double				fT;
			TScale				fScale;
};


//#pragma mark  Conversion functions
constexpr double	kToc	(double k)
{
	return k - 273.15;
}

constexpr double	kTof	(double k)
{
	return (kToc(k) * 1.8) + 32.0;
}

constexpr double	cTok	(double c)
{
	return c + 273.15;
}

constexpr double 	fTok		(double f)
{
	return (f + 459.67) * (0.5555555555);
}


constexpr double 	fToc		(double f)
{
	return kToc(fTok(f));
}


constexpr double 	cTof		(double c)
{
	return kTof(cTok(c));
}


//#pragma mark  quasi literals

constexpr EMTemperature _kelvin(long double t)
{
	return EMTemperature(t, TScale::Kelvin);
}


constexpr EMTemperature _celsius(long double t)
{
	return EMTemperature(cTok(t), TScale::Celsius);
}


constexpr EMTemperature _fahrenheit(long double t)
{
	return EMTemperature(fTok(t), TScale::Fahrenheit);
}


//#pragma mark  literals
constexpr EMTemperature operator "" _kelvin (long double t)
{
	return _kelvin(t);
}

constexpr EMTemperature operator "" _celsius (long double t)
{
	return _celsius(t);
}

constexpr EMTemperature operator "" _fahrenheit (long double t)
{
	return _fahrenheit(t);
}

//#pragma mark operators

EMTemperature operator + (const EMTemperature& t1, const EMTemperature& t2);
EMTemperature operator - (const EMTemperature& t1, const EMTemperature& t2);

bool operator == (const EMTemperature& t1, const EMTemperature& t2);
bool operator != (const EMTemperature& t1, const EMTemperature& t2);


// Constructors

constexpr EMTemperature	::	EMTemperature()
	:
	fT(-1.0),
	fScale(TScale::Kelvin)
	{}


constexpr EMTemperature	::	EMTemperature(double temp, TScale scale)
	:
	fT(temp),
	fScale(scale)
	{}



//};	// end namespace


#endif // EM_TEMPERATURE_H
