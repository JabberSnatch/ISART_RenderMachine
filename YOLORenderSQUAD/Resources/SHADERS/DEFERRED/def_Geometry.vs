#version 420

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

uniform mat4 u_WorldMatrix;

layout(std140, binding = 42)
uniform PVMatrices
{
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
} pvMatrices;


out VS_OUTPUT
{
	vec3 v_WorldPosition;
	vec3 v_Normal;
	vec2 v_TexCoords;
	vec3 v_Tangent;
	vec3 v_Bitangent;
} OUT;


void main(void)
{
	mat3 tiWorldMatrix = transpose(inverse(mat3(u_WorldMatrix)));
	
	gl_Position = pvMatrices.u_ProjectionMatrix * pvMatrices.u_ViewMatrix * u_WorldMatrix * vec4(a_Position, 1);

	OUT.v_WorldPosition = vec3(u_WorldMatrix * vec4(a_Position, 1.0));
	OUT.v_TexCoords = vec2(a_TexCoords.x, -a_TexCoords.y);
	OUT.v_Normal = normalize(tiWorldMatrix * a_Normal);
	OUT.v_Tangent = normalize(tiWorldMatrix * a_Tangent);
	OUT.v_Bitangent = normalize(tiWorldMatrix * a_Bitangent);
}
