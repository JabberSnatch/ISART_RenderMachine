#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "Matrix.hpp"
#include "Vec3.hpp"
#include "Quaternion.hpp"

struct Transform
{
	Transform()
		:Position(0.f), Rotation(), Scale(1.f) {}
	
	Vec3		Position;
	Quaternion	Rotation;
	Vec3		Scale;

	// Combines two transforms. 
	// Since they hold rotations, this operator is non-commutative
	Transform	operator * (const Transform& _other) const
	{
		Transform result;
		result.Position	= Position + (Rotation * _other.Position);
		result.Rotation = Rotation * _other.Rotation;
		result.Scale	= Scale * _other.Scale;
		return result;
	}

	auto	GetMatrix() const -> Matrix;
	auto	GetInverseMatrix() const -> Matrix;
};


#endif /*__TRANSFORM_HPP__*/