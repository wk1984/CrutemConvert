#ifndef EM_POINT_H
#define EM_POINT_H

/*
	A minimal (but precise) copy of EMPoint for portability and potential
	future augmentation.
*/


class EMPoint {
public:
			float				x;
			float				y;

								EMPoint();
								EMPoint(float x, float y);
								EMPoint(const EMPoint&);

			EMPoint&			operator=(const EMPoint& other);
};


//  Implementation

inline
EMPoint::EMPoint()
	:
	x(0.0f),
	y(0.0f)
{
}


inline
EMPoint::EMPoint(float x, float y)
	:
	x(x),
	y(y)
{
}


inline
EMPoint::EMPoint(const EMPoint& other)
	:
	x(other.x),
	y(other.y)
{
}


inline EMPoint&
EMPoint::operator=(const EMPoint& other)
{
	x = other.x;
	y = other.y;
	return *this;
}





#endif // EM_POINT_H

