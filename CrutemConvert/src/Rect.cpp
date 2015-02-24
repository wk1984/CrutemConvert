// Heavily redacted from Haiku version which holds this copyright:
/*
 * Copyright 2001-2014 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Frans van Nispen
 *		John Scipione, jscipione@gmail.com
 */

#include <algorithm>
#include <stdio.h>

#include "Point.h"
#include "Rect.h"


void
EMRect::InsetBy(float dx, float dy)
{
	 left += dx;
	 right -= dx;
	 top += dy;
	 bottom -= dy;
}


void
EMRect::OffsetBy(float dx, float dy)
{
	 left += dx;
	 right += dx;
	 top += dy;
	 bottom += dy;
}


void
EMRect::OffsetTo(float x, float y)
{
	 right = (right - left) + x;
	 left = x;
	 bottom = (bottom - top) + y;
	 top=y;
}


void
EMRect::PrintToStream() const
{
	printf("EMRect(l:%.1f, t:%.1f, r:%.1f, b:%.1f)\n", left, top, right, bottom);
}


bool
EMRect::operator==(EMRect other) const
{
	return left == other.left && right == other.right &&
		top == other.top && bottom == other.bottom;
}


bool
EMRect::operator!=(EMRect other) const
{
	return !(*this == other);
}


EMRect
EMRect::operator&(EMRect other) const
{
	return EMRect(std::max(left, other.left), std::max(top, other.top),
		std::min(right, other.right), std::min(bottom, other.bottom));
}


EMRect
EMRect::operator|(EMRect other) const
{
	return EMRect(std::min(left, other.left), std::min(top, other.top),
		std::max(right, other.right), std::max(bottom, other.bottom));
}


bool
EMRect::Intersects(EMRect rect) const
{
	if (!IsValid() || !rect.IsValid())
		return false;

	return !(rect.left > right || rect.right < left
		|| rect.top > bottom || rect.bottom < top);
}


bool
EMRect::Contains(EMPoint point) const
{
	return point.x >= left && point.x <= right
		&& point.y >= top && point.y <= bottom;
}


bool
EMRect::Contains(EMRect rect) const
{
	return rect.left >= left && rect.right <= right
		&& rect.top >= top && rect.bottom <= bottom;
}

