#version 420

const vec4 g_DefaultColor = vec4(1.0, 1.0, 1.0, 1.0);

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;
layout (location = 2) in vec3 a_Normal;

uniform mat4 u_WorldMatrix;

layout(std140, binding = 42)
uniform PVMatrices
{
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
} pvMatrices;

out VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
} OUT;

void main(void)
{
	mat3 tiWorldMatrix = transpose(inverse(mat3(u_WorldMatrix)));
	mat3 tiWVPMatrix = transpose(inverse(mat3(pvMatrices.u_ProjectionMatrix * pvMatrices.u_ViewMatrix * u_WorldMatrix)));
	
	gl_Position = pvMatrices.u_ProjectionMatrix * pvMatrices.u_ViewMatrix * u_WorldMatrix * vec4(a_Position, 1);
	OUT.v_Color = vec4(abs(a_Normal), 1.0f);
	OUT.v_Normal = normalize(tiWVPMatrix * a_Normal);
}
