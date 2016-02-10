#version 150

in VS_OUTPUT
{
	vec4 v_Color;
	vec2 v_TexCoords;
} IN[];

out GS_OUTPUT
{
	vec4 v_Color;
	vec2 v_TexCoords;
} OUT;


