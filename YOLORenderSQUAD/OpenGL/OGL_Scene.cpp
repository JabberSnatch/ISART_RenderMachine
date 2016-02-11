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

	//for (auto&& mesh : m_Meshes)
		//mesh->Render(m_MatricesBuffer);
	for(auto&& model : m_Models)
	{
		// WIP CODE ================
		OGL_Shader* ms = &model->GetMesh(0).GetShader();

		static GLfloat lightDirection[3] = { 0.f, 0.f, 1.f };
		static GLfloat Ia[3] = { .5f, .5f, .5f };
		static GLfloat Id[3] = { 1.f, 1.f, 1.f };
		static GLfloat Is[3] = { 1.f, 1.f, 1.f };
		glUniform3fv(ms->GetUniform("IN_LIGHT.Direction"), 1, lightDirection);
		glUniform3fv(ms->GetUniform("IN_LIGHT.Ia"), 1, Ia);
		glUniform3fv(ms->GetUniform("IN_LIGHT.Id"), 1, Id);
		glUniform3fv(ms->GetUniform("IN_LIGHT.Is"), 1, Is);
		glUniform3fv(ms->GetUniform("u_ViewPosition"), 1, m_Camera.Position.ToStdVec().data());
		glUniform3fv(ms->GetUniform("u_LightDirection"), 1, lightDirection);
		// =========================

		model->Render(m_MatricesBuffer);
	}
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
