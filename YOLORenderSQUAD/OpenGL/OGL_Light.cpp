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
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Position"), 1, m_Direction.ToStdVec().data());
		break;
	case SPOT:
		typeName = "Spot";
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Position"), 1, m_Direction.ToStdVec().data());
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Direction"), 1, m_Direction.ToStdVec().data());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Cutoff"), m_Cutoff);
		break;
	default: return;
	}

	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Ia"), 1, m_Ia.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Id"), 1, m_Id.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Is"), 1, m_Is.ToStdVec().data());
}