#include "OGL_Scene.hpp"


OGL_Scene::~OGL_Scene()
{
	FreeOGLResources();
}



auto
OGL_Scene::CreateBuffers() -> void
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


auto
OGL_Scene::FreeOGLResources() -> void
{
	if (m_MatricesBuffer)
		glDeleteBuffers(1, &m_MatricesBuffer);
	if (m_LightsBuffer)
		glDeleteBuffers(1, &m_LightsBuffer);
}


auto
OGL_Scene::Render() -> void
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(GLfloat), m_Matrices[PERSPECTIVE]);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), m_Camera.GetInverseMatrix().data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// SET SHADERS UP WITH LIGHTS AND VIEW
	{
		std::list<OGL_Shader*> shaders;
		for (auto& model : m_Models)
		{
			for (auto& mesh : model->GetMeshes())
			{
				OGL_Shader* shader = mesh.GetShader();
				if (std::find(shaders.begin(), shaders.end(), shader) == shaders.end())
					shaders.emplace_back(shader);
			}
		}

		/*
		{
			unsigned int lightCounts[OGL_Light::COUNT]; memset(lightCounts, 0, OGL_Light::COUNT * sizeof(unsigned int));
			for (std::list<OGL_Shader*>::iterator ite = shaders.begin(); ite != shaders.end(); ++ite)
			{
				for (OGL_Light& light : m_Lights)
					light.BindIntoShader(*ite, lightCounts[light.m_Type]++);
			
				glUniform1ui((*ite)->GetUniform("u_DirectionalCount"), lightCounts[OGL_Light::DIRECTIONAL]);
				glUniform1ui((*ite)->GetUniform("u_PointCount"), lightCounts[OGL_Light::POINT]);
				glUniform1ui((*ite)->GetUniform("u_SpotCount"), lightCounts[OGL_Light::SPOT]);
				glUniform3fv((*ite)->GetUniform("u_ViewPosition"), 1, m_Camera.Position.ToStdVec().data());
			}
		}
		*/
		{
			unsigned int lightCounts[OGL_Light::COUNT]; memset(lightCounts, 0, OGL_Light::COUNT * sizeof(unsigned int));
			for (std::list<OGL_Shader*>::iterator ite = shaders.begin(); ite != shaders.end(); ++ite)
			{
				glUniform3fv((*ite)->GetUniform("u_ViewPosition"), 1, m_Camera.Position.ToStdVec().data());
			}

			for (OGL_Light& light : m_Lights)
				light.BindIntoBuffer(m_LightsBuffer, lightCounts[light.m_Type]++);

			glBindBuffer(GL_UNIFORM_BUFFER, m_LightsBuffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint), &lightCounts[OGL_Light::DIRECTIONAL]);
			glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(GLuint), sizeof(GLuint), &lightCounts[OGL_Light::POINT]);
			glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(GLuint), sizeof(GLuint), &lightCounts[OGL_Light::SPOT]);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_LightsBuffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, 42, m_MatricesBuffer);
		}
	}

	for(auto&& model : m_Models)
	{
		model->Render(m_MatricesBuffer);
	}
}


#define PI 3.14159265359
auto
OGL_Scene::CenterCamera(Vec3 _min, Vec3 _max, float _FOV) -> void
{
	/*
	tan(a) = sin(a) / cos(a) car tan = oppose / adjacent
	dou
		adjacent = oppose / tangent

	dans notre cas: a = FOVy / 2

	H = max.y - min.y;
	distance = (H/2) / tan(a) => (H/2) * cotan(a)
	CAMERA.z = (H/2) / tan(FOVy/2)
	*/

	float H = _max.y - _min.y;
	float W = _max.x - _min.x;
	float D = _max.z - _min.z;

	m_Camera.Position.x = _min.x + W / 2.f;
	m_Camera.Position.y = _min.y + H / 2.f;
	m_Camera.Position.z = D / 2.f + ((H / 2.f) / (float)tan(_FOV * (PI / 180.) / 2.));
}


auto
OGL_Scene::SetPerspectiveMatrix(GLfloat* _matrix) -> void
{
	memcpy(m_Matrices[PERSPECTIVE], _matrix, 16 * sizeof(GLfloat));
}


auto
OGL_Scene::SetViewMatrix(GLfloat* _matrix) -> void
{
	memcpy(m_Matrices[VIEW], _matrix, 16 * sizeof(GLfloat));
}
