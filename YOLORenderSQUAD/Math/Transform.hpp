#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "Matrix.hpp"
#include "Vec3.hpp"

struct Transform
{
	Transform()
		:Position(0.f), Rotation(0.f), Scale(1.f) {}

	Vec3	Position;
	Vec3	Rotation;
	Vec3	Scale;

	auto	GetMatrix() -> Matrix;
	auto	GetInverseMatrix() -> Matrix;
};


#endif /*__TRANSFORM_HPP__*/