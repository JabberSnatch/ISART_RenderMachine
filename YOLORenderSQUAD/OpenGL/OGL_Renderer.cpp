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
	Viewport vp = camera->GetViewport();
	glViewport(vp.x, vp.y, vp.width, vp.height);

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

	glUseProgram(0);
}


void
OGL_Renderer::ImGui_RenderDrawLists(ImDrawData* _data)
{
	GLuint VBO, IBO;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);

	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.f, width, height, 0.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	for (int n = 0; n < _data->CmdListsCount; ++n)
	{
		const ImDrawList*	cmd_list = _data->CmdLists[n];
		ImDrawIdx*			idx_buffer = cmd_list->IdxBuffer.Data;
		ImDrawVert*			vtx_buffer = cmd_list->VtxBuffer.Data;

		glBufferData(GL_ARRAY_BUFFER, cmd_list->VtxBuffer.Size * (4 * sizeof(GLfloat) + sizeof(GLint)), vtx_buffer, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)(4 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); ++cmd_i)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			unsigned int elem_count = pcmd->ElemCount;

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, elem_count * sizeof(GLushort), idx_buffer, GL_STREAM_DRAW);

			if (pcmd->UserCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((GLint)pcmd->ClipRect.x, (GLint)(height - pcmd->ClipRect.w),
						  (GLint)(pcmd->ClipRect.z - pcmd->ClipRect.x), (GLint)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, elem_count, GL_UNSIGNED_SHORT, nullptr);
			}

			idx_buffer += elem_count;
		}
		/*
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); ++cmd_i)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			unsigned int elem_count = pcmd->ElemCount;
			
			if (pcmd->UserCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)pcmd->TextureId);
				glScissor((GLint)pcmd->ClipRect.x, (GLint)(height - pcmd->ClipRect.w),
					(GLint)(pcmd->ClipRect.z - pcmd->ClipRect.x), (GLint)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				
				glBegin(GL_TRIANGLES);
				for (int i = 0; i < elem_count; ++i)
				{
					ImDrawVert vertex = vtx_buffer[*(idx_buffer + i)];
					glColor4bv((GLbyte*)&vertex.col);
					glTexCoord2f(vertex.uv.x, vertex.uv.y);
					glVertex2f(vertex.pos.x, vertex.pos.y);
				}
				glEnd();
				
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			idx_buffer += elem_count;
		}
		*/
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
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

