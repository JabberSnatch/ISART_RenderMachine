#version 420

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;
layout (location = 2) in vec3 a_Normal;

layout(std140, binding = 42)
uniform PVMatrices
{
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
} pvMatrices;

out vec3 TexCoords;


void main()
{
	gl_Position = pvMatrices.u_ProjectionMatrix * mat4(mat3(pvMatrices.u_ViewMatrix)) * vec4(a_Position, 1.0);
	TexCoords = a_Position;	
}