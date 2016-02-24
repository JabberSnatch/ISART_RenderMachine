#include "OGL_Light.hpp"

void
OGL_Light::BindIntoShader(OGL_Shader* _shader, unsigned int _index)
{
	std::string typeName = "";
	std::string index = std::to_string(_index);
	switch (m_Type)
	{
	case DIRECTIONAL:
		typeName = "Directional";
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Direction"), 1, m_Direction.ToStdVec().data());
		break;
	case POINT:
		typeName = "Point";
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Position"), 1, m_Position.ToStdVec().data());
		
		// These values describe a light with a range of 20 units
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Constant"), Constant());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Linear"), Linear());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Quadratic"), Quadratic());
		break;
	case SPOT:
		typeName = "Spot";
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Position"), 1, m_Position.ToStdVec().data());
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Direction"), 1, m_Direction.ToStdVec().data());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].OuterCutoff"), Cutoff());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].InnerCutoff"), InnerCutoff());
		break;
	default: return;
	}

	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Ia"), 1, m_Ia.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Id"), 1, m_Id.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Is"), 1, m_Is.ToStdVec().data());
}


void
OGL_Light::BindIntoBuffer(GLuint _buffer, unsigned int _index)
{
	glBindBuffer(GL_UNIFORM_BUFFER, _buffer);

	GLint baseOffset = m_ComputeLightOffset(_index);
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), m_Ia.ToArray().get());
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), m_Id.ToArray().get());
	glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + 2 * (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), m_Is.ToArray().get());

	baseOffset += 3 * 4 * sizeof(GLfloat);
	switch (m_Type)
	{
	case DIRECTIONAL:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), m_Direction.ToArray().get());
	} break;
	case POINT:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), m_Position.ToArray().get());
		baseOffset += 4 * sizeof(GLfloat);
		GLfloat constant = Constant(), linear = Linear(), quadratic = Quadratic();
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(GLfloat), &constant);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + sizeof(GLfloat), sizeof(GLfloat), &linear);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + 2 * sizeof(GLfloat), sizeof(GLfloat), &quadratic);
	} break;
	case SPOT:
	{
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, 3 * sizeof(GLfloat), m_Position.ToArray().get());
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + (4 * sizeof(GLfloat)), 3 * sizeof(GLfloat), m_Direction.ToArray().get());
		baseOffset += 2 * (4 * sizeof(GLfloat));
		GLfloat innerCutoff = InnerCutoff(), cutoff = Cutoff();
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset, sizeof(GLfloat), &innerCutoff);
		glBufferSubData(GL_UNIFORM_BUFFER, baseOffset + sizeof(GLfloat), sizeof(GLfloat), &cutoff);
	} break;
	default: break;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


GLint
OGL_Light::m_ComputeLightOffset(unsigned int _index)
{
	GLint result = ARRAY_OFFSET;

	switch (m_Type)
	{
	case DIRECTIONAL:
		result += _index * DIRLIGHT_SIZE;
		break;
	case POINT:
		result += DIRLIGHT_SIZE * MAX_LIGHT_COUNT + _index * POINTLIGHT_SIZE;
		break;
	case SPOT:
		result += (DIRLIGHT_SIZE + POINTLIGHT_SIZE) * MAX_LIGHT_COUNT + _index * SPOTLIGHT_SIZE;
		break;
	default: break;
	}

	return result;
}