#ifndef __OGL_FRAMEBUFFER_HPP__
#define __OGL_FRAMEBUFFER_HPP__

#include <string>
#include <map>

#include "glew/include/GL/glew.h"





// ABSTRACT: OGL_Framebuffer is made to manage one OpenGL framebuffer and 
//			 all of its render targets.
// NOTE: For now, a render target can only be a texture.
class OGL_Framebuffer
{
public:
	struct RenderTargetDesc
	{
		bool	Valid = false;
		GLuint	TargetName = 0;
		GLenum	Format = 0;
		GLenum	AttachmentPoint = 0;
	};
	typedef std::map<std::string, OGL_Framebuffer::RenderTargetDesc> RenderTargetMap_t;

	OGL_Framebuffer(int _width, int _height)
		: m_Width(_width), m_Height(_height) 
	{
		glGenFramebuffers(1, &m_Framebuffer);
	}
	OGL_Framebuffer() = delete;
	OGL_Framebuffer(const OGL_Framebuffer&) = delete;
	OGL_Framebuffer(OGL_Framebuffer&&) = delete;
	~OGL_Framebuffer()
	{
		RemoveAllColorAttachment();
		RemoveDepthStencilAttachment();
		glDeleteFramebuffers(1, &m_Framebuffer);
	}

	void	Bind() const;
	void	Activate() const;
	void	ActivateColorAttachments(std::initializer_list<std::string> _targets) const;
	void	Unbind() const;

	void	Resize(int _width, int _height);

	bool	ValidateFramebuffer();

	void	ClearAllAttachments();

	void					SetDepthStencilAttachment(GLenum _format, GLenum _attachment);
	const RenderTargetDesc*	GetDepthStencilAttachment() const;
	void					RemoveDepthStencilAttachment();

	void					EmplaceColorAttachment(const std::string& _name, GLenum _format, GLenum _attachmentPoint);
	const RenderTargetDesc*	GetColorAttachment(const std::string& _name) const;
	void					RemoveAllColorAttachment();

	auto	operator = (const OGL_Framebuffer&) -> OGL_Framebuffer& = delete;
	auto	operator = (OGL_Framebuffer&&) -> OGL_Framebuffer& = delete;

private:
	RenderTargetDesc	CreateRenderTarget(GLenum _format, GLenum _attachmentPoint);

	bool				m_IsValid = false;

	int					m_Width;
	int					m_Height;

	GLuint				m_Framebuffer = 0;

	RenderTargetDesc	m_DepthStencil;
	RenderTargetMap_t	m_RenderTargets;

};


#endif /*__OGL_FRAMEBUFFER_HPP__*/