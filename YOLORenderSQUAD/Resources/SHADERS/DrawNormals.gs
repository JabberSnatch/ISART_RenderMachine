#version 420

layout(triangles) in;
layout(line_strip, max_vertices = 10) out;

in VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
} IN[];

out VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
} OUT;

void main()
{
	int i;
	for(i = 0; i < gl_in.length(); ++i)
	{
		OUT.v_Normal = IN[i].v_Normal;
		OUT.v_Color = IN[i].v_Color;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();

		gl_Position = gl_in[i].gl_Position + vec4(normalize(IN[i].v_Normal) * 0.5, 0.0);
		EmitVertex();

		EndPrimitive();
	}
}