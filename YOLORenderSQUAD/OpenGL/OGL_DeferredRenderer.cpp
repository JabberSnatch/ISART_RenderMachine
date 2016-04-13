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
	{ DEPTH, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT }
};


//OGL_DeferredRenderer::OGL_DeferredRenderer(int _width, int _height)
//	: m_Framebuffer(_width, _height)
//{}


void
OGL_DeferredRenderer::Initialize()
{
	OGL_Renderer::Initialize();

	//for (int i = 0; i < DEPTH; ++i)
	//{
	//	RenderTargetDesc target = AvailableTargets[i];
	//	m_Framebuffer.EmplaceColorAttachment(TargetToString((RenderTarget)i), target.InternalFormat, GL_COLOR_ATTACHMENT0 + i);
	//}
	//
	//m_Framebuffer.SetDepthStencilAttachment(AvailableTargets[DEPTH].InternalFormat, GL_DEPTH_ATTACHMENT);
	//m_Framebuffer.ValidateFramebuffer();
	
	glGenFramebuffers(1, &m_FrameBuffer);
	AllocateRenderTextures();
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	GLenum checkResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (checkResult != GL_FRAMEBUFFER_COMPLETE)
		printf("Error : Incomplete framebuffer \n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.vs", GL_VERTEX_SHADER);
	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.fs", GL_FRAGMENT_SHADER);

	m_LightingPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.vs", GL_VERTEX_SHADER);
	m_LightingPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_BlinnPhong.fs", GL_FRAGMENT_SHADER);

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

	LoadPVMatrices(camera);

	//// For each light in scene
	//// Bind light data to buffer
	//LoadLightData(_scene->LightsMap());

	// TODO: Use stencil buffer to flag pixels that should be overwritten by sky
	
	static GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glDrawBuffers(3, attachments);
	
	//m_Framebuffer.Bind();
	//m_Framebuffer.Activate();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const RenderObjectMap_t& renderObjectsMap = _scene->RenderObjectsMap();
	for (auto& pair : renderObjectsMap)
	{
		OGL_RenderObject* object = reinterpret_cast<OGL_RenderObject*>(pair.second);
		for(auto& mesh : object->Meshes())
			mesh.SetShader(&m_GeometryPass);

		pair.second->Render();
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	LightingPass(_scene->LightsMap(), cameraTransform);
	//DebugDrawBuffer(NORMAL);
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
	//m_Framebuffer.Resize(_width, _height);

	FreeRenderTextures();
	m_Width = _width;
	m_Height = _height;
	AllocateRenderTextures();
}


void
OGL_DeferredRenderer::LightingPass(const LightMap_t& _lights, const Transform& _cam)
{
	GLboolean DepthTest = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// TODO: Split lighting shader into one shader per type of light
	m_LightingPass.EnableShader();
	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		std::string targetName = TargetToString((RenderTarget)i);
		std::string uniformName = "u_" + targetName + "Map";

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_RenderTextures[i]);
		glUniform1i(m_LightingPass.GetUniform(uniformName), i);
		
		//const OGL_Framebuffer::RenderTargetDesc* desc = m_Framebuffer.GetColorAttachment(targetName);
	}

	LoadLightData(_lights);
	glUniform3fv(m_LightingPass.GetUniform("u_ViewPosition"), 1, _cam.Position.ToStdVec().data());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (DepthTest) glEnable(GL_DEPTH_TEST);
}


void
OGL_DeferredRenderer::DebugDrawBuffer(RenderTarget _target)
{
	GLboolean DepthTest = glIsEnabled(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RenderTextures[_target]);
	
	m_QuadShader.EnableShader();
	glUniform1i(m_QuadShader.GetUniform("source"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	if (DepthTest) glEnable(GL_DEPTH_TEST);
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

			// TODO: GL error display
			auto error = glGetError();
			if (error != GL_NO_ERROR)
			{
				printf("Error when creating a render texture : ");
				if (error == GL_INVALID_OPERATION)
					printf("INVALID OPERATION\n");
				else if (error == GL_INVALID_ENUM)
					printf("INVALID ENUM\n");
				else if (error == GL_INVALID_VALUE)
					printf("INVALID VALUE\n");
				else
					printf("wat.\n");
			}

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


std::string
OGL_DeferredRenderer::TargetToString(RenderTarget _target)
{
	switch (_target)
	{
	case POSITION:
		return "Position";
	case NORMAL:
		return "Normal";
	case DIFFUSE_SPEC:
		return "DiffuseSpec";
	case DEPTH:
		return "Depth";
	default:
		return "";
	}
}
