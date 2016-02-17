#ifndef __OGL_LIGHT_HPP__
#define __OGL_LIGHT_HPP__

#include "Vec3.hpp"
#include "OGL_Shader.hpp"


class OGL_Light
{
public:
	enum LIGHT_TYPE {DIRECTIONAL, POINT, SPOT, COUNT};

	OGL_Light() = delete;
	OGL_Light(LIGHT_TYPE _type)
		:m_Type(_type) {}
	OGL_Light(const OGL_Light&) = default;
	OGL_Light(OGL_Light&&) = default;
	~OGL_Light() = default;

	void	BindIntoShader(OGL_Shader* _shader, unsigned int _index);

	auto	operator = (const OGL_Light&)->OGL_Light& = delete;
	auto	operator = (OGL_Light&&)->OGL_Light& = delete;


	LIGHT_TYPE	m_Type = DIRECTIONAL;

	Vec3	m_Ia = Vec3(0.0);
	Vec3	m_Id = Vec3(0.0);
	Vec3	m_Is = Vec3(0.0);

	Vec3	m_Position = Vec3::Zero();
	Vec3	m_Direction = Vec3::Zero();
	float	m_Cutoff = 90.f;
};


#endif /*__OGL_LIGHT_HPP__*/