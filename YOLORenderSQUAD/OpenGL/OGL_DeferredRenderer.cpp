#include "OGL_DeferredRenderer.hpp"

#include "Logger.hpp"

#include "IRenderObject.hpp"
#include "OGL_RenderObject.hpp"
#include "Camera.hpp"


const std::vector<OGL_DeferredRenderer::RenderTargetParam>
OGL_DeferredRenderer::AvailableTargets =
{
	{ "Position", GL_RGB32F },
	{ "Normal", GL_RGB32F },
	{ "DiffuseSpec", GL_RGBA8 },
	{ "Depth", GL_DEPTH_COMPONENT24 }
};


OGL_DeferredRenderer::OGL_DeferredRenderer(int _width, int _height)
	: m_Framebuffer(_width, _height)
{}


void
OGL_DeferredRenderer::Initialize()
{
	OGL_Renderer::Initialize();

	for (size_t i = 0; i < AvailableTargets.size() - 1; ++i)
	{
		RenderTargetParam target = AvailableTargets[i];
		m_Framebuffer.EmplaceColorAttachment(target.Identifier, target.InternalFormat, GL_COLOR_ATTACHMENT0 + (int)i);
	}
	m_Framebuffer.SetDepthStencilAttachment((--AvailableTargets.end())->InternalFormat, GL_DEPTH_ATTACHMENT);
	m_Framebuffer.ValidateFramebuffer();
	
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

	// TODO: Use stencil buffer to flag pixels that should be overwritten by sky
	
	m_Framebuffer.Bind();
	m_Framebuffer.Activate();
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

	m_Framebuffer.ClearAllAttachments();
}


void
OGL_DeferredRenderer::Resize(int _width, int _height)
{
	m_Framebuffer.Resize(_width, _height);

	m_Width = _width;
	m_Height = _height;
}


void
OGL_DeferredRenderer::LightingPass(const LightMap_t& _lights, const Transform& _cam)
{
	GLboolean DepthTest = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// TODO: Split lighting shader into one shader per type of light
	m_LightingPass.EnableShader();
	for (size_t i = 0; i < AvailableTargets.size() - 1; ++i)
	{
		const std::string& targetName = AvailableTargets[i].Identifier;
		std::string uniformName = "u_" + targetName + "Map";
	
		const OGL_Framebuffer::RenderTargetDesc* desc = m_Framebuffer.GetColorAttachment(targetName);
		if (desc)
		{
			glActiveTexture(GL_TEXTURE0 + (int)i);
			glBindTexture(GL_TEXTURE_2D, desc->TargetName);
			glUniform1i(m_LightingPass.GetUniform(uniformName), (int)i);
		}
	}

	LoadLightData(_lights);
	glUniform3fv(m_LightingPass.GetUniform("u_ViewPosition"), 1, _cam.Position.ToStdVec().data());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	for (int i = 0; i < AvailableTargets.size() - 1; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (DepthTest) glEnable(GL_DEPTH_TEST);
}


void
OGL_DeferredRenderer::DebugDrawBuffer(int _target)
{
	GLboolean DepthTest = glIsEnabled(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);

	const std::string& targetName = AvailableTargets[_target].Identifier;
	const OGL_Framebuffer::RenderTargetDesc* desc = m_Framebuffer.GetColorAttachment(targetName);
	if (desc)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, desc->TargetName);

		m_QuadShader.EnableShader();
		glUniform1i(m_QuadShader.GetUniform("source"), 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
		LOG_WARNING("No render target found for " + targetName);
	

	if (DepthTest) glEnable(GL_DEPTH_TEST);
}

