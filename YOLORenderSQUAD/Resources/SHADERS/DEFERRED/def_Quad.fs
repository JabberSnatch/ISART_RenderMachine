#version 420

in vec2 texCoord;

uniform sampler2D source;

out vec4 outColor;

void main()
{
    outColor = texture(source, texCoord);
}