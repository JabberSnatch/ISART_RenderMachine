#include "OGL_DeferredRenderer.hpp"

#include "IRenderObject.hpp"
#include "OGL_RenderObject.hpp"
#include "Camera.hpp"


const OGL_DeferredRenderer::RenderTargetDesc
OGL_DeferredRenderer::AvailableTargets[RENDER_TARGET_COUNT] =
{
	{ POSITION, GL_RGB32F, GL_RGB, GL_FLOAT },
	{ NORMAL, GL_RGB32F, GL_RGB, GL_FLOAT },
	{ DIFFUSE_SPEC, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
	{ DEPTH, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT }
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

	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.vs", GL_VERTEX_SHADER);
	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.fs", GL_FRAGMENT_SHADER);

	m_QuadShader.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.vs", GL_VERTEX_SHADER);
	m_QuadShader.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.fs", GL_FRAGMENT_SHADER);
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

	//// For each light in scene
	//// Bind light data to buffer
	//LoadLightData(_scene->LightsMap());

	static GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glDrawBuffers(3, attachments);
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const RenderObjectMap_t& renderObjectsMap = _scene->RenderObjectsMap();
	for (auto& pair : renderObjectsMap)
	{
		OGL_RenderObject* object = reinterpret_cast<OGL_RenderObject*>(pair.second);
		for(auto& mesh : object->Meshes())
			mesh.SetShader(&m_GeometryPass);

		pair.second->Render();
	}


#if 0
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBuffer);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, 1280, 740, 0, 0, 640, 370, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//glReadBuffer(GL_COLOR_ATTACHMENT1);
	//glBlitFramebuffer(0, 0, 1280, 720, 640, 0, 1280, 360, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return;
#endif


	// Render each object using DeferredIlluminationShader
	// Render a screen space quad using combination shader
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	DebugDrawBuffer(POSITION);
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
OGL_DeferredRenderer::DebugDrawBuffer(RenderTarget _target)
{
	GLboolean DepthTest = glIsEnabled(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE0, m_RenderTextures[_target]);

	m_QuadShader.EnableShader();
	glUniform1i(m_QuadShader.GetUniform("source"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	glBindTexture(GL_TEXTURE0, 0);

	if (DepthTest) glEnable(GL_DEPTH_TEST);
}


void
OGL_DeferredRenderer::DrawScreenQuad()
{

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
			//glTexImage2D(GL_TEXTURE_2D, 0, desc.InternalFormat, m_Width, m_Height, 0, desc.Format, desc.Type, nullptr);

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
