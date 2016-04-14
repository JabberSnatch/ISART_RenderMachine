#version 420

in vec2 texCoord;

uniform sampler2D u_Source;

out vec4 outColor;

void main()
{
    outColor = texture(u_Source, texCoord);
}