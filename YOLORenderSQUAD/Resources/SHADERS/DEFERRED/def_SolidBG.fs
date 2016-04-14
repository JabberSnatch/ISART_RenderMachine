#version 420

in vec2 texCoord;
uniform vec3 u_Color;
layout(location = 0) out vec4 FragmentColor;

void main()
{
    FragmentColor = vec4(u_Color, 1.0);
}