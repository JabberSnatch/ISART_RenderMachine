#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "Component.hpp"

#include "Vec3.hpp"
#include "Matrix.hpp"

struct Viewport
{
	int x;
	int y;
	int width;
	int height;
};


class Camera final
	:public Component
{
public:
	Camera() = default;
	Camera(const Camera&) = delete;
	Camera(Camera&&) = delete;
	virtual ~Camera() = default;

	void	Initialize(const Viewport& _viewport, float _FOV = 60.f, float _near = .1f, float _far = 1000.f);

	void			ComputePerspective();
	const Matrix&	PerspectiveMatrix() const;
	// Watch out for the FOUR matrices creations and assignment
	Matrix			ViewMatrix() const;

	void	CenterOnBounds(Vec3 _min, Vec3 _max);

	void	SetFOV(float _v) { m_FOV = _v; }
	void	SetNear(float _v) { m_Near = _v; }
	void	SetFar(float _v) { m_Far = _v; }
	void	SetViewport(const Viewport& _v) 
	{ 
		m_Viewport = _v; 
		m_AspectRatio = _v.width / (float)_v.height; 
		ComputePerspective();
	}

	const Viewport&	GetViewport() { return m_Viewport; }

	auto	operator = (const Camera&) -> Camera& = delete;
	auto	operator = (Camera&&) -> Camera& = delete;

private:

	Viewport	m_Viewport;
	float		m_FOV;
	float		m_Near;
	float		m_Far;
	float		m_AspectRatio;

	Matrix	m_Perspective;
	
	// It would be great to have a way for a node to signal its components whenever it moves
	// Matrix	m_View;

};


#endif /*__CAMERA_HPP__*/