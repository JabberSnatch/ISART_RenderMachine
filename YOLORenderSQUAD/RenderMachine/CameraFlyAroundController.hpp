#ifndef __CAMERA_FLY_AROUND_CONTROLLER_HPP__
#define __CAMERA_FLY_AROUND_CONTROLLER_HPP__

#include "IDynamicObject.hpp"

#include "Node.hpp"
#include "Transform.hpp"

#include "Device.hpp"
#include "imgui.h"


class CameraFlyAroundController
	: public IDynamicObject
{
public:
	CameraFlyAroundController() = default;
	CameraFlyAroundController(const CameraFlyAroundController&) = delete;
	CameraFlyAroundController(CameraFlyAroundController&&) = delete;
	~CameraFlyAroundController() = default;

	virtual void Update(double _deltaTime)
	{
		Transform& localTransform = getNode()->LocalTransform();
		Quaternion rot = localTransform.Rotation;

		static float speed = 5.f;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_W))
			localTransform.Position += rot * Vec3::Forward() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_S))
			localTransform.Position += rot * Vec3::Forward() * (float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_A))
			localTransform.Position += rot * Vec3::Right() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_D))
			localTransform.Position += rot * Vec3::Right() * (float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_E))
			localTransform.Position += rot * Vec3::Up() * -(float)_deltaTime * speed;
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_Q))
			localTransform.Position += rot * Vec3::Up() * (float)_deltaTime * speed;
		if (INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_LEFT))
		{
			iVec2 deltaPos = INPUT->GetMouseDelta();
			localTransform.Rotation = Quaternion(-deltaPos.x * (float)_deltaTime * speed * 2.f, Vec3::Up()) * localTransform.Rotation;
			localTransform.Rotation = Quaternion(-deltaPos.y * (float)_deltaTime * speed * 2.f, localTransform.Rotation * Vec3::Right()) * localTransform.Rotation;
		}
		if (INPUT->IsKeyDown(EKEY_CODE::KEY_RETURN))
		{
			localTransform.Position = Vec3::Zero();
			localTransform.Rotation = Quaternion();
		}

		ImGui::Text("%f, %f, %f", localTransform.Position.x, localTransform.Position.y, localTransform.Position.z);
		Vec3 eulerRotation = rot.toEuler();
		ImGui::Text("%f, %f, %f", eulerRotation.x, eulerRotation.y, eulerRotation.z);

	}

	auto	operator = (const CameraFlyAroundController&) -> CameraFlyAroundController& = delete;
	auto	operator = (CameraFlyAroundController&&) -> CameraFlyAroundController& = delete;

};


#endif /*__CAMERA_FLY_AROUND_CONTROLLER_HPP__*/