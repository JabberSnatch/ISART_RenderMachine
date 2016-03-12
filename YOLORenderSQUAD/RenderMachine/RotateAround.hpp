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
		static float speed = 5.f;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_UP))
			localTransform.Position += Vec3::Up() * (float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_DOWN))
			localTransform.Position += Vec3::Up() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_LEFT))
			localTransform.Position += Vec3::Right() * (float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_RIGHT))
			localTransform.Position += Vec3::Right() * -(float)_deltaTime * speed;
		//if (INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_LEFT))
		//{
		//	iVec2 deltaPos = INPUT->GetMouseDelta();
		//	localTransform.Position.x += deltaPos.x * (float)_deltaTime * speed;
		//	localTransform.Position.y += deltaPos.y * (float)_deltaTime * speed;
		//}
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_RETURN))
		{
			localTransform.Position = Vec3::Zero();
			localTransform.Rotation = Quaternion();
		}
	}

	auto	operator = (const RotateAround&) -> RotateAround& = delete;
	auto	operator = (RotateAround&&) -> RotateAround& = delete;
};


#endif /*__ROTATE_AROUND_HPP__*/