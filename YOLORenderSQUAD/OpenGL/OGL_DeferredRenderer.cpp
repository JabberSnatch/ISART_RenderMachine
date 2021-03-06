#include "OGL_DeferredRenderer.hpp"

#include "Logger.hpp"

#include "IRenderObject.hpp"
#include "OGL_RenderObject.hpp"
#include "Camera.hpp"
#include "OGL_Skybox.hpp"
#include "OGL_ErrorLog.hpp"


#ifdef _IMGUI
// TODO: Do not leave imgui here
#include "imgui.h"
#endif
static float g_Exposure = 1.f;

const std::vector<OGL_DeferredRenderer::RenderTargetParam>
OGL_DeferredRenderer::AvailableTargets =
{
	{ "Position", GL_RGB32F, GL_COLOR_ATTACHMENT0},
	{ "Normal", GL_RGB32F, GL_COLOR_ATTACHMENT1 },
	{ "DiffuseSpec", GL_RGBA32F, GL_COLOR_ATTACHMENT2 },
	{ "PostLighting", GL_RGBA32F, GL_COLOR_ATTACHMENT3 },
	{ "DepthStencil", GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT }
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
		const RenderTargetParam& target = AvailableTargets[i];
		m_Framebuffer.EmplaceColorAttachment(target.Identifier, target.InternalFormat, target.AttachmentPoint);
	}
	{
		const RenderTargetParam& target = *(--AvailableTargets.end());
		m_Framebuffer.SetDepthStencilAttachment(target.InternalFormat, target.AttachmentPoint);
	}
	m_Framebuffer.ValidateFramebuffer();
	
	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.vs", GL_VERTEX_SHADER);
	m_GeometryPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Geometry.fs", GL_FRAGMENT_SHADER);

	m_LightingPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.vs", GL_VERTEX_SHADER);
	m_LightingPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_BlinnPhong.fs", GL_FRAGMENT_SHADER);

	m_SolidBackgroundPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.vs", GL_VERTEX_SHADER);
	m_SolidBackgroundPass.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_SolidBG.fs", GL_FRAGMENT_SHADER);

	m_QuadShader.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_Quad.vs", GL_VERTEX_SHADER);
	m_QuadShader.LoadShaderAndCompile("../Resources/SHADERS/DEFERRED/def_ToneMapping.fs", GL_FRAGMENT_SHADER);
}


void
OGL_DeferredRenderer::Render(const Scene* _scene)
{
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDepthFunc(GL_LEQUAL);

	{
		int unhandledErrorCount = OGL_ErrorLog::ClearErrorStack();
		if (unhandledErrorCount > 0)
			LOG_WARNING(std::to_string(unhandledErrorCount) + " GL errors were unhandled during the last frame.");
	}

	// Get Camera from scene
	Camera* camera = _scene->MainCamera();
	Transform cameraTransform = camera->getNode()->WorldTransform();
	SetViewport(camera);

	LoadPVMatrices(camera);

	// GEOMETRY PASS
	// DEPTH
	// STENCIL
	{
		m_Framebuffer.Bind();
		m_Framebuffer.ActivateColorAttachments({ "Position", "Normal", "DiffuseSpec" });
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xff);
		glStencilFunc(GL_ALWAYS, 0xf0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		const RenderObjectMap_t& renderObjectsMap = _scene->RenderObjectsMap();
		for (auto& pair : renderObjectsMap)
		{
			OGL_RenderObject* object = reinterpret_cast<OGL_RenderObject*>(pair.second);
			for(auto& mesh : object->Meshes())
				mesh.SetShader(&m_GeometryPass);

			pair.second->Render();
		}
	}

	// LIGHTING PASS
	// NO DEPTH 
	// NO STENCIL
	{
		m_Framebuffer.ActivateColorAttachments({ "PostLighting" });
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_STENCIL_TEST);

		LightingPass(_scene->LightsMap(), cameraTransform, { "Position", "Normal", "DiffuseSpec" });
	}
	
	// BACKGROUND PASS
	// NO DEPTH
	// STENCIL
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0xf0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		ISkybox* sky = _scene->Skybox();
		if (sky)
			sky->Render();
		else
		{
			Vec3 color(0.23f, 0.45f, 0.67f);
			m_SolidBackgroundPass.EnableShader();
			glUniform3fv(m_SolidBackgroundPass.GetUniform("u_Color"), 1, color.ToArray().get());
		}
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glUseProgram(0);
	}


#ifdef _IMGUI
	ImGui::SliderFloat("Exposure", &g_Exposure, 0.001f, 10.f);
#endif
	// FINAL PASS
	// NO DEPTH
	// NO STENCIL
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer.GetColorAttachment("PostLighting")->TargetName);

		m_QuadShader.EnableShader();
		glUniform1i(m_QuadShader.GetUniform("u_Source"), 0);
		glUniform1f(m_QuadShader.GetUniform("u_Exposure"), g_Exposure);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
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
OGL_DeferredRenderer::LightingPass(const LightMap_t& _lights, const Transform& _cam, std::initializer_list<std::string> _sourceRenderTargets)
{
	// TODO: Split lighting shader into one shader per type of light
	m_LightingPass.EnableShader();

	int boundTargetCount = 0;
	for (const std::string* ite = _sourceRenderTargets.begin(); ite < _sourceRenderTargets.end(); ++ite)
	{
		const std::string& targetName = *ite;
		std::string uniformName = "u_" + targetName + "Map";

		const OGL_Framebuffer::RenderTargetDesc* desc = m_Framebuffer.GetColorAttachment(targetName);
		if (desc)
		{
			glActiveTexture(GL_TEXTURE0 + boundTargetCount);
			glBindTexture(GL_TEXTURE_2D, desc->TargetName);
			glUniform1i(m_LightingPass.GetUniform(uniformName), boundTargetCount);
			++boundTargetCount;
		}
		else
			LOG_WARNING("Render target " + targetName + " was not found as a color attachment.");
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
		glUniform1i(m_QuadShader.GetUniform("u_Source"), 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
		LOG_WARNING("No render target found for " + targetName);
	

	if (DepthTest) glEnable(GL_DEPTH_TEST);
}

