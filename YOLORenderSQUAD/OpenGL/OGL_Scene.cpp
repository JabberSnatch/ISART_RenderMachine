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
}


auto
OGL_Scene::FreeOGLResources() -> void
{
	glDeleteBuffers(1, &m_MatricesBuffer);
}


auto
OGL_Scene::Render() -> void
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(GLfloat), m_Matrices[PERSPECTIVE]);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), m_Camera.GetInverseMatrix().data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for(auto&& model : m_Models)
	{
		// WIP CODE ================
		OGL_Shader* ms = &model->GetMesh(0).GetShader();

		//static GLfloat lightPosition[4] = { 0.f, 100.f, 100.f, 1.f };
		static GLfloat lightPosition[4] = { 0.f, 0.f, 1.f, 0.f };
		//static GLfloat Ia[3] = { .2f, .2f, .2f };
		static GLfloat Ia[3] = { 1.f, 1.f, 1.f };
		static GLfloat Id[3] = { 1.f, 1.f, 1.f };
		//static GLfloat Is[3] = { .8f, .8f, .8f };
		static GLfloat Is[3] = { 1.f, 1.f, 1.f };
		glUniform3fv(ms->GetUniform("IN_LIGHT.Ia"), 1, Ia);
		glUniform3fv(ms->GetUniform("IN_LIGHT.Id"), 1, Id);
		glUniform3fv(ms->GetUniform("IN_LIGHT.Is"), 1, Is);
		glUniform3fv(ms->GetUniform("u_ViewPosition"), 1, m_Camera.Position.ToStdVec().data());
		glUniform4fv(ms->GetUniform("u_LightPosition"), 1, lightPosition);
		// =========================

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
