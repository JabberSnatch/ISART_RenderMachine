#include "Camera.hpp"

#include "Constants.hpp"
#include "Node.hpp"


void
Camera::Initialize(const Viewport& _viewport, float _FOV, float _near, float _far)
{
	m_Viewport = _viewport;
	m_FOV = _FOV;
	m_Near = _near;
	m_Far = _far;
	m_AspectRatio = _viewport.width / (float)_viewport.height;
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
	Matrix result;

	Transform& worldTransform = c_getNode()->WorldTransform();
	Vec3 right = worldTransform.Rotation * Vec3::Right();
	Vec3 up = worldTransform.Rotation * Vec3::Up();
	Vec3 forward = worldTransform.Rotation * Vec3::Forward();
	Vec3 translate = worldTransform.Position * -1.f;

	result.Set(0, 0, right.x);
	result.Set(1, 0, right.y);
	result.Set(2, 0, right.z);

	result.Set(0, 1, up.x);
	result.Set(1, 1, up.y);
	result.Set(2, 1, up.z);

	result.Set(0, 2, forward.x);
	result.Set(1, 2, forward.y);
	result.Set(2, 2, forward.z);
	
	result.Set(3, 0, right.dot(translate));
	result.Set(3, 1, up.dot(translate));
	result.Set(3, 2, forward.dot(translate));

	return result;
}


void
Camera::CenterOnBounds(Vec3 _min, Vec3 _max)
{
	float H = _max.y - _min.y;
	float W = _max.x - _min.x;
	float D = _max.z - _min.z;

	Transform& t = getNode()->LocalTransform();

	t.Position.x = _min.x + W / 2.f;
	t.Position.y = _min.y + H / 2.f;
	t.Position.z = D / 2.f + ((H / 2.f) / (float)tan(m_FOV * (MC::PI() / 180.) / 2.));
}
