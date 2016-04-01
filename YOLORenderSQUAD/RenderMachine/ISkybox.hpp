#ifndef __ISKYBOX_HPP__
#define __ISKYBOX_HPP__

#include "IRenderObject.hpp"


class ISkybox
	: public IRenderObject
{
public:
	enum E_FACE
	{
		RIGHT = 0, LEFT, TOP, BOTTOM, BACK, FRONT, COUNT
	};
	ISkybox() = default;
	ISkybox(const ISkybox&) = delete;
	ISkybox(ISkybox&&) = delete;
	virtual ~ISkybox() = default;

	virtual void	Attach(Node* _node) override;
	virtual void	Render() = 0;

	void			SetTexturePath(E_FACE _face, const std::string& _path) { m_TexturesPath[_face] = _path; }

	auto	operator = (const ISkybox&) -> ISkybox& = delete;
	auto	operator = (ISkybox&&) -> ISkybox& = delete;

protected:
	std::string		m_TexturesPath[6];

};


#endif /*__ISKYBOX_HPP__*/