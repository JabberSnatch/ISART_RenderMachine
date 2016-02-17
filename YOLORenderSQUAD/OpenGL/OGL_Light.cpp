#include "OGL_Light.hpp"

#define PI 3.14159265359
#define Deg2Rad PI / 180.


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
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Constant"), 1.f);
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Linear"), 0.22f);
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Quadratic"), 0.20f);
		break;
	case SPOT:
		typeName = "Spot";
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Position"), 1, m_Position.ToStdVec().data());
		glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Direction"), 1, m_Direction.ToStdVec().data());
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].OuterCutoff"), cos(m_Cutoff * Deg2Rad));
		glUniform1f(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].InnerCutoff"), cos((m_Cutoff * 0.8) * Deg2Rad));
		break;
	default: return;
	}

	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Ia"), 1, m_Ia.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Id"), 1, m_Id.ToStdVec().data());
	glUniform3fv(_shader->GetUniform("u_" + typeName + "Lights[" + index + "].Is"), 1, m_Is.ToStdVec().data());
}