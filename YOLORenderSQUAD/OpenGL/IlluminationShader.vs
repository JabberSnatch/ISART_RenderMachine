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

uniform vec3 u_ViewPosition;
uniform vec4 u_LightPosition;

out VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
	vec2 v_TexCoords;
	vec3 v_ViewDirection;
	vec3 v_LightDirection;
} OUT;

void main(void)
{
	gl_Position = pvMatrices.u_ProjectionMatrix * pvMatrices.u_ViewMatrix * u_WorldMatrix * vec4(a_Position, 1);

	OUT.v_Color = vec4(abs(a_Normal), 1.0f);
	OUT.v_TexCoords = vec2(a_TexCoords.x, -a_TexCoords.y);

	mat3 tiWorldMatrix = transpose(inverse(mat3(u_WorldMatrix)));
	OUT.v_Normal = normalize(tiWorldMatrix * a_Normal);
	OUT.v_ViewDirection = normalize(u_ViewPosition - (tiWorldMatrix * a_Position));

	if (u_LightPosition.w == 0.0)
		OUT.v_LightDirection = u_LightPosition.xyz;
	else
		OUT.v_LightDirection = (u_LightPosition.xyz / u_LightPosition.w) - (mat4(u_WorldMatrix) * vec4(a_Position, 1)).xyz;
	
	OUT.v_LightDirection = normalize(OUT.v_LightDirection);
}
