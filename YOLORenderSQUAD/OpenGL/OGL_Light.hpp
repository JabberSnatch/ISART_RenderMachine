#ifndef __OGL_LIGHT_HPP__
#define __OGL_LIGHT_HPP__

#include "Vec3.hpp"
#include "OGL_Shader.hpp"
#include "Constants.hpp"

#define MAX_LIGHT_COUNT 50
#define DIRLIGHT_SIZE (4 * 4 * sizeof(GLfloat))
#define POINTLIGHT_SIZE (4 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat))
#define SPOTLIGHT_SIZE (5 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat))

#define ARRAY_OFFSET 4 * sizeof(GLuint)

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
	void	BindIntoBuffer(GLuint _buffer, unsigned int _index);

	GLfloat Constant()		{ return 1.f; }
	GLfloat Linear()		{ return .22f; }
	GLfloat Quadratic()		{ return 0.20f; }
	GLfloat InnerCutoff()	{ return (GLfloat)cos((m_Cutoff * 0.5f) * MC::Deg2Rad()); }
	GLfloat Cutoff()		{ return (GLfloat)cos(m_Cutoff * MC::Deg2Rad()); }

	auto	operator = (const OGL_Light&)->OGL_Light& = delete;
	auto	operator = (OGL_Light&&)->OGL_Light& = delete;


	LIGHT_TYPE	m_Type = DIRECTIONAL;

	Vec3	m_Ia = Vec3(0.0);
	Vec3	m_Id = Vec3(0.0);
	Vec3	m_Is = Vec3(0.0);

	Vec3	m_Position = Vec3::Zero();
	Vec3	m_Direction = Vec3::Zero();
	float	m_Cutoff = 90.f;

private:
	GLint	m_ComputeLightOffset(unsigned int _index);
};


#endif /*__OGL_LIGHT_HPP__*/