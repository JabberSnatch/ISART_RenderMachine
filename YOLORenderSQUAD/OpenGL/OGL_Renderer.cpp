#include "OGL_Renderer.hpp"

#include <list>

#include "OGL_Shader.hpp"
#include "OGL_RenderObject.hpp"

#include "Camera.hpp"
#include "IRenderObject.hpp"
#include "Light.hpp"
#include "ISkybox.hpp"


void
OGL_Renderer::Initialize()
{
	glGenBuffers(1, &m_MatricesBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2 * 16 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_LightsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_LightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, ARRAY_OFFSET +
		MAX_LIGHT_COUNT * (DIRLIGHT_SIZE + POINTLIGHT_SIZE + SPOTLIGHT_SIZE), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void
OGL_Renderer::Render(const Scene* _scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	// Lists all used shaders
	// For each render objects shader
		// Bind uniforms (i.e. Camera world position)
	const RenderObjectMap_t& renderObjectsMap = _scene->RenderObjectsMap();
	std::list<OGL_Shader*> shaders;
	for (auto& pair : renderObjectsMap)
	{
		OGL_RenderObject* object = (OGL_RenderObject*)pair.second;
		for (auto& mesh : object->Meshes())
		{
			OGL_Shader* shader = mesh.GetShader();
			if (std::find(shaders.begin(), shaders.end(), shader) == shaders.end())
				shaders.emplace_back(shader);
		}
	}
	for (std::list<OGL_Shader*>::iterator ite = shaders.begin(); ite != shaders.end(); ++ite)
	{
		(*ite)->EnableShader();
		glUniform3fv((*ite)->GetUniform("u_ViewPosition"), 1, cameraTransform.Position.ToStdVec().data());
		glUseProgram(0);
	}
	
	// Render skybox
	// For each render object
		// object.Render()
	for (auto& pair : renderObjectsMap)
		pair.second->Render();
	if (_scene->Skybox())
		_scene->Skybox()->Render();

	glUseProgram(0);
}


void
OGL_Renderer::Shutdown()
{
	if (m_MatricesBuffer)
		glDeleteBuffers(1, &m_MatricesBuffer);
	if (m_LightsBuffer)
		glDeleteBuffers(1, &m_LightsBuffer);
}


void
OGL_Renderer::SetViewport(const Camera* _camera)
{
	Viewport vp = _camera->GetViewport();
	glViewport(vp.x, vp.y, vp.width, vp.height);
}


void
OGL_Renderer::LoadPVMatrices(const Camera* _camera)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(GLfloat), _camera->PerspectiveMatrix().data);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), _camera->ViewMatrix().data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, MATRICES_BUFFER_BASE, m_MatricesBuffer);
}


void
OGL_Renderer::LoadLightData(const LightMap_t& _lights)
{
	unsigned int lightCounts[Light::COUNT]; 
	memset(lightCounts, 0, Light::COUNT * sizeof(unsigned int));

	for (LightMap_t::const_iterator ite = _lights.cbegin(); ite != _lights.cend(); ++ite)
		BindLightIntoBuffer(*ite->second, lightCounts[ite->second->m_Type]++);

	glBindBuffer(GL_UNIFORM_BUFFER, m_LightsBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint), &lightCounts[Light::DIRECTIONAL]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(GLuint), sizeof(GLuint), &lightCounts[Light::POINT]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(GLuint), sizeof(GLuint), &lightCounts[Light::SPOT]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_BUFFER_BASE, m_LightsBuffer);
}


void
OGL_Renderer::BindLightIntoBuffer(const Light& _light, unsigned int _lightIndex) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_LightsBuffer);

	GLint baseOffset = ComputeLightOffset(_light, _lightIndex);
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), _light.m_Ia.ToArray().get());
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), _light.m_Id.ToArray().get());
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + 2 * (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), _light.m_Is.ToArray().get());

	baseOffset += 3 * 4 * sizeof(GLfloat);
	switch (_light.m_Type)
	{
	case Light::DIRECTIONAL:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), _light.m_Direction.ToArray().get());
	} break;
	case Light::POINT:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), _light.m_Position.ToArray().get());
		baseOffset += 4 * sizeof(GLfloat);
		GLfloat constant = _light.Constant(), linear = _light.Linear(), quadratic = _light.Quadratic();
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(GLfloat), &constant);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + sizeof(GLfloat), sizeof(GLfloat), &linear);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + 2 * sizeof(GLfloat), sizeof(GLfloat), &quadratic);
	} break;
	case Light::SPOT:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), _light.m_Position.ToArray().get());
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), _light.m_Direction.ToArray().get());
		baseOffset += 2 * (4 * sizeof(GLfloat));
		GLfloat innerCutoff = _light.InnerCutoff(), cutoff = _light.Cutoff();
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(GLfloat), &innerCutoff);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + sizeof(GLfloat), sizeof(GLfloat), &cutoff);
	} break;
	default: break;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


GLint
OGL_Renderer::ComputeLightOffset(const Light& _light, unsigned int _lightIndex) const
{
	GLint result = ARRAY_OFFSET;

	switch (_light.m_Type)
	{
	case Light::DIRECTIONAL:
		result += _lightIndex * DIRLIGHT_SIZE;
		break;
	case Light::POINT:
		result += DIRLIGHT_SIZE * MAX_LIGHT_COUNT + _lightIndex * POINTLIGHT_SIZE;
		break;
	case Light::SPOT:
		result += (DIRLIGHT_SIZE + POINTLIGHT_SIZE) * MAX_LIGHT_COUNT + _lightIndex * SPOTLIGHT_SIZE;
		break;
	default: break;
	}

	return result;
}

