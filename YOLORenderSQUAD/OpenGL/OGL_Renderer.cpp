#include "OGL_Renderer.hpp"

#include <list>

#include "OGL_Shader.hpp"
#include "OGL_RenderObject.hpp"

#include "Camera.hpp"
#include "IRenderObject.hpp"
#include "Light.hpp"


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
	// Get Camera from scene
	Camera* camera = _scene->MainCamera();
	Transform cameraTransform = camera->getNode()->WorldTransform();

	// Retrieve Perspective and View matrices from it
	// Bind matrices to buffer
	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(GLfloat), camera->PerspectiveMatrix().data);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), cameraTransform.GetInverseMatrix().data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// For each light in scene
		// Bind light data to buffer
	unsigned int lightCounts[Light::COUNT]; memset(lightCounts, 0, Light::COUNT * sizeof(unsigned int));
	const LightMap_t& lightMap = _scene->LightsMap();
	for (LightMap_t::const_iterator ite = lightMap.cbegin(); ite != lightMap.cend(); ++ite)
		BindLightIntoBuffer(*ite->second, lightCounts[ite->second->m_Type]++);

	glBindBuffer(GL_UNIFORM_BUFFER, m_LightsBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint), &lightCounts[Light::DIRECTIONAL]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(GLuint), sizeof(GLuint), &lightCounts[Light::POINT]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(GLuint), sizeof(GLuint), &lightCounts[Light::SPOT]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_BUFFER_BASE, m_LightsBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, MATRICES_BUFFER_BASE, m_MatricesBuffer);

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
		glUniform3fv((*ite)->GetUniform("u_ViewPosition"), 1, cameraTransform.Position.ToStdVec().data());
	
	// For each render object
		// object.Render()
	for (auto& pair : renderObjectsMap)
		pair.second->Render();
}


void
OGL_Renderer::ImGui_RenderDrawLists(ImDrawData* _data)
{
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.f, width, height, 0.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();



	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPopAttrib();
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

