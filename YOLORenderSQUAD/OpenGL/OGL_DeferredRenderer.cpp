#include "OGL_DeferredRenderer.hpp"

#include "Camera.hpp"


const OGL_DeferredRenderer::RenderTargetDesc
OGL_DeferredRenderer::AvailableTargets[RENDER_TARGET_COUNT] =
{
	{ POSITION, GL_RGB32F, GL_RGB, GL_FLOAT },
	{ NORMAL, GL_RGB32F, GL_RGB, GL_FLOAT },
	{ DIFFUSE_SPEC, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
	{ DEPTH, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24 }
};


void
OGL_DeferredRenderer::Initialize()
{
	OGL_Renderer::Initialize();

	glGenFramebuffers(1, &m_FrameBuffer);
	AllocateRenderTextures();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	GLenum checkResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (checkResult != GL_FRAMEBUFFER_COMPLETE)
		printf("Error : Incomplete framebuffer \n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void
OGL_DeferredRenderer::Render(const Scene* _scene)
{
	// Get Camera from scene
	Camera* camera = _scene->MainCamera();
	Transform cameraTransform = camera->getNode()->WorldTransform();
	SetViewport(camera);

	// Retrieve Perspective and View matrices from it
	// Bind matrices to buffer
	LoadPVMatrices(camera);

	// For each light in scene
	// Bind light data to buffer
	LoadLightData(_scene->LightsMap());


	// Render each object using DeferredIlluminationShader
	// Render a screen space quad using combination shader
	DrawScreenQuad();
}


void
OGL_DeferredRenderer::Shutdown()
{
	OGL_Renderer::Shutdown();

	FreeRenderTextures();
	glDeleteFramebuffers(1, &m_FrameBuffer);
}


void
OGL_DeferredRenderer::Resize(int _width, int _height)
{
	FreeRenderTextures();
	m_Width = _width;
	m_Height = _height;
	AllocateRenderTextures();
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


void	
OGL_DeferredRenderer::AllocateRenderTextures()
{
	if (m_FrameBuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

		glGenTextures(RENDER_TARGET_COUNT, m_RenderTextures);
		int		attachmentCount = RENDER_TARGET_COUNT - 1;
		GLuint*	attachments = new GLuint[attachmentCount];

		for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
		{
			const RenderTargetDesc& desc = AvailableTargets[i];
			
			GLuint attachment;
			if (i != DEPTH)
			{
				attachment = GL_COLOR_ATTACHMENT0 + i;
				attachments[i] = attachment;
			}
			else
				attachment = GL_DEPTH_ATTACHMENT;

			glBindTexture(GL_TEXTURE_2D, m_RenderTextures[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, desc.InternalFormat, m_Width, m_Height);

			// TODO ASAP: Logger system
			auto error = glGetError();
			if (error != GL_NO_ERROR)
				printf("Error when creating a render texture");

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RenderTextures[i], 0);

			error = glGetError();
			if (error != GL_NO_ERROR)
				printf("Error when attaching a render texture");

		}

		glDrawBuffers(attachmentCount, attachments);
		auto error = glGetError();
		if (error != GL_NO_ERROR)
			printf("Error when creating a render texture");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		delete attachments;
	}
}


void	
OGL_DeferredRenderer::FreeRenderTextures()
{
	if (m_FrameBuffer)
	{
		glDeleteTextures(RENDER_TARGET_COUNT, m_RenderTextures);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

		GLuint	attachments[RENDER_TARGET_COUNT];

		for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
			attachments[i] = GL_NONE;

		glDrawBuffers(RENDER_TARGET_COUNT, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
