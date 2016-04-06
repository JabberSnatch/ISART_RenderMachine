#ifndef __ROTATE_AROUND_HPP__
#define __ROTATE_AROUND_HPP__

#include "IDynamicObject.hpp"

#include "Node.hpp"
#include "Transform.hpp"

#include "Device.hpp"

class RotateAround final
	:public IDynamicObject
{
public:
	RotateAround() = default;
	RotateAround(const RotateAround&) = delete;
	RotateAround(RotateAround&&) = delete;
	virtual ~RotateAround() = default;

	virtual void Update(double _deltaTime)
	{
		Transform& localTransform = getNode()->LocalTransform();
		localTransform.Rotation = Quaternion((float)_deltaTime * 79.f, Vec3::Up()) * localTransform.Rotation;
		//localTransform.Rotation = Quaternion((float)_deltaTime * 57.f, Vec3::Right()) * localTransform.Rotation;
		//localTransform.Rotation = Quaternion((float)_deltaTime * 83.f, Vec3::Forward()) * localTransform.Rotation;
	}

	auto	operator = (const RotateAround&) -> RotateAround& = delete;
	auto	operator = (RotateAround&&) -> RotateAround& = delete;
};


#endif /*__ROTATE_AROUND_HPP__*/