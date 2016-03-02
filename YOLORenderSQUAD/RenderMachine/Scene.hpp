#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Node.hpp"

class Camera; // :public Component


class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	~Scene() = default;

	auto	operator = (const Scene&) -> Scene& = delete;
	auto	operator = (Scene&&) -> Scene& = delete;

private:
	Node*		m_Root = nullptr;
	Camera*		m_MainCamera;

};


#endif /*__SCENE_HPP__*/