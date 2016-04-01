#include "OGL_Skybox.hpp"

#include "OGL_TextureLoader.hpp"
#include "OGL_Mesh.hpp"

#include "Node.hpp"

OGL_Skybox::~OGL_Skybox()
{
	if (m_Box) delete m_Box;
}


void
OGL_Skybox::AllocateResources()
{
	m_Cubemap = OGL_TextureLoader::Get()->GetCubeMap(m_TexturesPath, true);
}


void
OGL_Skybox::Render()
{
	OGL_Shader* shader = m_Box->GetShader();

	//glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDepthMask(GL_FALSE);

	shader->EnableShader();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(shader->GetUniform("skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Cubemap);

	glBindVertexArray(m_Box->m_BufferObjects[OGL_Mesh::VAO]);
	glDrawElements(GL_TRIANGLES, m_Box->m_IndicesCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDepthMask(GL_TRUE);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
}


void
OGL_Skybox::SetBoxMesh(const MeshData& _source, OGL_Shader* _shader)
{
	m_Box = new OGL_Mesh(_source);
	m_Box->SetShader(_shader);
}
