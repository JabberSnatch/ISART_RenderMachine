#include "OGL_DeferredRenderer.hpp"


void
OGL_DeferredRenderer::Initialize()
{
	OGL_Renderer::Initialize();

	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

	glGenTextures(RENDER_TARGET_COUNT, m_RenderTextures);
	GLuint	attachments[RENDER_TARGET_COUNT];

	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		const RenderTargetDesc& desc = AvailableTargets[i];
		GLuint	attachment = GL_COLOR_ATTACHMENT0 + i;

		glBindTexture(GL_TEXTURE_2D, m_RenderTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, desc.InternalFormat, m_Width, m_Height, 0, desc.Format, desc.Type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RenderTextures[i], 0);
	
		attachments[i] = attachment;
	}

	glDrawBuffers(RENDER_TARGET_COUNT, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void
OGL_DeferredRenderer::Render(const Scene* _scene)
{
	// Get Camera and set viewport
	// Set matrices and lights
	// Render each object using DeferredIlluminationShader
	// Render a screen space quad using combination shader
	DrawScreenQuad();
}


void
OGL_DeferredRenderer::Shutdown()
{
	OGL_Renderer::Shutdown();

	glDeleteTextures(RENDER_TARGET_COUNT, m_RenderTextures);
	glDeleteFramebuffers(1, &m_FrameBuffer);
}


void
OGL_DeferredRenderer::DrawScreenQuad()
{
	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE0 + i, m_RenderTextures[i]);
	}
	
	// Enable appropriate shader
	// Render quad

	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
		glBindTexture(GL_TEXTURE0 + i, 0);
}
