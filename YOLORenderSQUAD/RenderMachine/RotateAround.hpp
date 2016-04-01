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
		Quaternion or = localTransform.Rotation;
		//localTransform.Rotation = Quaternion((float)_deltaTime * 79.f, Vec3::Up()) * localTransform.Rotation;
		//localTransform.Rotation = Quaternion((float)_deltaTime * 57.f, Vec3::Right()) * localTransform.Rotation;
		//localTransform.Rotation = Quaternion((float)_deltaTime * 83.f, Vec3::Forward()) * localTransform.Rotation;
		static float speed = 5.f;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_UP))
			localTransform.Position += or * Vec3::Forward() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_DOWN))
			localTransform.Position += or * Vec3::Forward() * (float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_LEFT))
			localTransform.Position += or * Vec3::Right() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_RIGHT))
			localTransform.Position += or * Vec3::Right() * (float)_deltaTime * speed;
		if (INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_LEFT))
		{
			iVec2 deltaPos = INPUT->GetMouseDelta();
			//localTransform.Rotation.x += deltaPos.x * (float)_deltaTime * speed;
			//localTransform.Rotation.y += deltaPos.y * (float)_deltaTime * speed;
			//localTransform.Rotation = Quaternion(Vec3(deltaPos.x, deltaPos.y, 0.f) * (float)_deltaTime * speed) * localTransform.Rotation;
			localTransform.Rotation = Quaternion(-deltaPos.x * (float)_deltaTime * speed * 2.f, Vec3::Up()) * localTransform.Rotation;
			localTransform.Rotation = Quaternion(-deltaPos.y * (float)_deltaTime * speed * 2.f, localTransform.Rotation * Vec3::Right()) * localTransform.Rotation;
		}
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