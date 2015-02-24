#ifndef EM_RECT_H
#define EM_RECT_H

/*
	A minimal (but precise) copy of BRect for portability and possible
	future augmentation.
*/


#include "Point.h"
#include "StdTypedefs.h"

class EMRect {
public:
			float				left;
			float				top;
			float				right;
			float				bottom;

								EMRect();
								EMRect(const EMRect& other);
								EMRect(float left, float top, float right,
									float bottom);
								EMRect(float side);

			EMRect&				operator=(const EMRect& other);

			void				PrintToStream() const;

	// Transformation
			void				InsetBy(float dx, float dy);
			void				OffsetBy(float dx, float dy);
			void				OffsetTo(float x, float y);

	// Comparison
			bool				operator==(EMRect other) const;
			bool				operator!=(EMRect other) const;

	// Intersection and union
			EMRect				operator&(EMRect other) const;
			EMRect				operator|(EMRect other) const;

			bool				IsValid() const;
			float				Width() const;
			float				Height() const;

			bool				Intersects(EMRect rect) const;
			bool				Contains(EMPoint point) const;
			bool				Contains(EMRect rect) const;
};


// #pragma mark - inline definitions

inline
EMRect::EMRect()
	:
	left(0),
	top(0),
	right(-1),
	bottom(-1)
{
}


inline
EMRect::EMRect(float left, float top, float right, float bottom)
	:
	left(left),
	top(top),
	right(right),
	bottom(bottom)
{
}


inline
EMRect::EMRect(const EMRect& other)
	:
	left(other.left),
	top(other.top),
	right(other.right),
	bottom(other.bottom)
{
}


inline
EMRect::EMRect(float side)
	:
	left(0),
	top(0),
	right(side - 1),
	bottom(side - 1)
{
}


inline EMRect&
EMRect::operator=(const EMRect& other)
{
	left = other.left;
	top = other.top;
	right = other.right;
	bottom = other.bottom;
	return *this;
}


inline bool
EMRect::IsValid() const
{
	return left <= right && top <= bottom;
}


inline float
EMRect::Width() const
{
	return right - left;
}


inline float
EMRect::Height() const
{
	return bottom - top;
}


#endif //EM_RECT_H
