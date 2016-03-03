#include "Camera.hpp"

#include "Constants.hpp"
#include "Node.hpp"


void
Camera::Initialize(float _aspectRatio, float _FOV, float _near, float _far)
{
	m_FOV = _FOV;
	m_Near = _near;
	m_Far = _far;
	m_AspectRatio = _aspectRatio;
}


void
Camera::ComputePerspective()
{
	float xymax = m_Near * (float)tan(m_FOV * (MC::PI() / 360.));
	float width = 2 * xymax;

	float depth = m_Far - m_Near;
	float q = -(m_Far + m_Near) / depth;
	float qn = -2 * (m_Far * m_Near) / depth;

	float w = (2 * m_Near / width) / m_AspectRatio;
	float h = 2 * m_Near / width;

	m_Perspective.Set(0, 0, w);
	m_Perspective.Set(1, 1, h);
	m_Perspective.Set(2, 2, q);
	m_Perspective.Set(2, 3, -1.f);
	m_Perspective.Set(3, 2, qn);
	m_Perspective.Set(3, 3, 0.f);
}


const Matrix&
Camera::PerspectiveMatrix() const
{
	return m_Perspective;
}


Matrix
Camera::ViewMatrix() const
{
	return c_getNode()->WorldTransform().GetInverseMatrix();
}

