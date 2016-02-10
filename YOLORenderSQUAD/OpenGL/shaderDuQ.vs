#version 430

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Normal;

uniform mat4 u_WorldMatrix;

layout(std140, binding = 42)
uniform PVMatrices
{
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
} pvMatrices;

layout(std140)//, binding = 42)
uniform Matrices
{
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
	mat4 u_WorldMatrix;
} matrices;

out VS_OUTPUT
{
	vec4 v_Color;
	vec2 v_TexCoords;
} OUT;

void main(void)
{
	vec4 position = vec4(a_Position, 1) + gl_InstanceID * 0.1;
	gl_Position = pvMatrices.u_ProjectionMatrix * pvMatrices.u_ViewMatrix * u_WorldMatrix * position;

	OUT.v_Color = a_Color;
	OUT.v_TexCoords = a_TexCoords;
}
