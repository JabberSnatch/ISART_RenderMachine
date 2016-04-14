#version 420

in vec2 texCoord;
uniform samplerCube u_Skybox;
layout(location = 0) out vec4 FragmentColor;

void main()
{
    FragmentColor = texture(u_Skybox, texCoord);
}