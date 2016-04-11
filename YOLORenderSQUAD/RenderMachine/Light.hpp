#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "Component.hpp"
#include "Vec3.hpp"
#include "Constants.hpp"


class Light
	:public Component
{
public:
	enum LIGHT_TYPE { DIRECTIONAL, POINT, SPOT, COUNT };

	Light() = default;
	Light(const Light&) = delete;
	Light(Light&&) = delete;
	virtual ~Light() = default;

	virtual void Attach(Node* _node);

	// TODO: If colors are in linear space, constant linear and quadratic are useless.
	//		 Replace them with a single range parameter and implement 1/range^2
	float	Constant() const { return 1.f; }
	float	Linear() const { return .22f; }
	float	Quadratic() const { return 0.20f; }
	float	InnerCutoff() const { return (float)cos((m_Cutoff * 0.5f) * MC::Deg2Rad()); }
	float	Cutoff() const { return (float)cos(m_Cutoff * MC::Deg2Rad()); }

	auto	operator = (const Light&) -> Light& = delete;
	auto	operator = (Light&&) -> Light& = delete;

	LIGHT_TYPE	m_Type = DIRECTIONAL;

	// What is this still doing here. 
	// Lights should use their node's position and rotation
	Vec3		m_Position = Vec3::Zero();
	Vec3		m_Direction = Vec3::Zero();
	
	Vec3		m_Ia = Vec3(0.f);
	Vec3		m_Id = Vec3(0.f);
	Vec3		m_Is = Vec3(0.f);

	float		m_Cutoff = 90.f;
	float		m_Range = 20.f;

};


#endif /*__LIGHT_HPP__*/