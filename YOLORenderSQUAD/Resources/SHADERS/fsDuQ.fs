#version 430

const vec4 defaultColor = vec4(1., 0.0, 0.0, 1.0);

in VS_OUTPUT
{
    vec2 v_TexCoords;
} IN;

uniform sampler2D u_TextureUnit;

out vec4 FragmentColor;

void main(void)
{
    vec4 texColor = texture(u_TextureUnit, IN.v_TexCoords);
    FragmentColor = defaultColor;
}
