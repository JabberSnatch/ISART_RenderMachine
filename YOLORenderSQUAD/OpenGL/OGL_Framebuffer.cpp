#include "OGL_Framebuffer.hpp"

#include <vector>

#include "Logger.hpp"
#include "OGL_ErrorLog.hpp"


void
OGL_Framebuffer::Bind() const
{
	// TODO: Should OGL_Framebuffer be able to let the user choose between different write modes ?
	// TODO: What about storing the currently bound framebuffer ?
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}


void
OGL_Framebuffer::Activate() const
{
	std::vector<GLenum> attachments;
	attachments.resize(m_RenderTargets.size());

	for (RenderTargetMap_t::const_iterator ite = m_RenderTargets.begin(); ite != m_RenderTargets.end(); ++ite)
	{
		int position = ite->second.AttachmentPoint - GL_COLOR_ATTACHMENT0;
		attachments[position] = ite->second.AttachmentPoint;
	}

	glDrawBuffers((GLsizei)attachments.size(), attachments.data());
}


void
OGL_Framebuffer::ActivateColorAttachments(std::initializer_list<std::string> _targets) const
{
	std::vector<GLenum> attachments;

	for (const std::string* ite = _targets.begin(); ite != _targets.end(); ++ite)
	{
		const RenderTargetDesc* target = GetColorAttachment(*ite);
		if (target)
			attachments.push_back(target->AttachmentPoint);
	}

	glDrawBuffers((GLsizei)attachments.size(), attachments.data());
}


void
OGL_Framebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void
OGL_Framebuffer::Resize(int _width, int _height)
{
	m_Width = _width;
	m_Height = _height;
	
	m_IsValid = false;
	{
		RenderTargetMap_t copy = m_RenderTargets;
		RemoveAllColorAttachment();

		for (RenderTargetMap_t::iterator ite = copy.begin(); ite != copy.end(); ++ite)
			EmplaceColorAttachment(ite->first, ite->second.Format, ite->second.AttachmentPoint);
	}

	{
		RenderTargetDesc copy = m_DepthStencil;
		RemoveDepthStencilAttachment();

		SetDepthStencilAttachment(copy.Format, copy.AttachmentPoint);
	}

	ValidateFramebuffer();
}


bool
OGL_Framebuffer::ValidateFramebuffer()
{
	Bind();

	m_IsValid = false;
	GLenum checkResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (checkResult == GL_FRAMEBUFFER_COMPLETE)
		m_IsValid = true;
	else
		LOG_ERROR("Incomplete framebuffer");

	Unbind();

	return m_IsValid;
}


void
OGL_Framebuffer::ClearAllAttachments()
{
	if (m_DepthStencil.Valid)
		RemoveDepthStencilAttachment();
	RemoveAllColorAttachment();
}


void
OGL_Framebuffer::SetDepthStencilAttachment(GLenum _format, GLenum _attachmentPoint)
{
	if (m_DepthStencil.Valid)
		RemoveDepthStencilAttachment();

	m_DepthStencil = CreateRenderTarget(_format, _attachmentPoint);
}


const OGL_Framebuffer::RenderTargetDesc*
OGL_Framebuffer::GetDepthStencilAttachment() const
{
	return &m_DepthStencil;
}


void
OGL_Framebuffer::RemoveDepthStencilAttachment()
{
	if (m_DepthStencil.Valid)
		glDeleteTextures(1, &m_DepthStencil.TargetName);

	m_DepthStencil.TargetName = 0;
}


void
OGL_Framebuffer::EmplaceColorAttachment(const std::string& _name, GLenum _format, GLenum _attachmentPoint)
{
	RenderTargetMap_t::iterator findResult = m_RenderTargets.find(_name);
	if (findResult == m_RenderTargets.end())
	{
		LOG_DEBUG("Creating color attachment " + _name);
		RenderTargetDesc desc = CreateRenderTarget(_format, _attachmentPoint);
		if (desc.Valid)
			m_RenderTargets.emplace(_name, desc);
		else
			LOG_WARNING("CreateRenderTarget produced an invalid descriptor.");
	}
	else
		LOG_WARNING("Render target " + _name + " already exists.");
}


const OGL_Framebuffer::RenderTargetDesc*
OGL_Framebuffer::GetColorAttachment(const std::string& _name) const
{
	const RenderTargetDesc* result = nullptr;
	RenderTargetMap_t::const_iterator findResult = m_RenderTargets.find(_name);
	if (findResult != m_RenderTargets.end())
		result = &findResult->second;
	else
		LOG_CRITICAL("Render target " + _name + " doesn't exists, returning nullptr");

	return result;
}


void
OGL_Framebuffer::RemoveAllColorAttachment()
{
	for (RenderTargetMap_t::iterator ite = m_RenderTargets.begin(); ite != m_RenderTargets.end(); ++ite)
		glDeleteTextures(1, &ite->second.TargetName);


	m_RenderTargets.clear();
}


OGL_Framebuffer::RenderTargetDesc
OGL_Framebuffer::CreateRenderTarget(GLenum _format, GLenum _attachmentPoint)
{
	Bind();

	RenderTargetDesc desc;
	desc.Valid = true;
	desc.AttachmentPoint = _attachmentPoint;
	desc.Format = _format;

	glGenTextures(1, &desc.TargetName);
	glBindTexture(GL_TEXTURE_2D, desc.TargetName);
	glTexStorage2D(GL_TEXTURE_2D, 1, _format, m_Width, m_Height);
	desc.Valid &= OGL_ERROR_LOG("glTexStorage2D");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, _attachmentPoint, GL_TEXTURE_2D, desc.TargetName, 0);
	desc.Valid &= OGL_ERROR_LOG("glFramebufferTexture2D");

	if (!desc.Valid)
		glDeleteTextures(1, &desc.TargetName);

	Unbind();

	return desc;
}
