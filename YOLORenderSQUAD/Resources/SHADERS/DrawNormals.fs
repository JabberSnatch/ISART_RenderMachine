#version 420

in VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
} IN;

void main()
{
    gl_FragColor = IN.v_Color;
}