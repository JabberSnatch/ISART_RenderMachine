#ifndef __IVEC2_HPP__
#define __IVEC2_HPP__

struct iVec2
{
	iVec2(int _x = 0, int _y = 0)
		:x(_x), y(_y) {}
	iVec2(const iVec2& _other)
		:x(_other.x), y(_other.y) {}

	iVec2	operator - (const iVec2& _other)
	{ return iVec2(x - _other.x, y - _other.y); }
	iVec2	operator + (const iVec2& _other)
	{ return iVec2(x + _other.x, y + _other.y); }

	iVec2&	operator = (const iVec2&) = default;
	iVec2&	operator = (iVec2&&) = default;

	int x;
	int y;
};

#endif // __IVEC2_HPP__