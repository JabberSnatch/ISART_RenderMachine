#version 420

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

in vec2 texCoord;
uniform sampler2D u_Source;
uniform float u_Exposure;
layout(location = 0) out vec4 FragmentColor;

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec4 U2Tonemapping(vec3 texColor)
{
    float ExposureBias = 2.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * texColor);

    vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
    vec3 color = curr * whiteScale;

    color = pow(color, vec3(1.0/2.2));
    return vec4(color, 1.0);
}

vec4 RichardBurgessDawson(vec3 texColor)
{
    vec3 x = max(vec3(0.0), texColor - 0.004);
    vec3 retColor = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    retColor = pow(retColor, vec3(2.2));
    return vec4(retColor, 1.0);
}

vec4 Reinhard(vec3 texColor)
{
   texColor = texColor/(1+texColor);
   return vec4(texColor,1.0);
}

void main()
{
    vec3 texColor = texture(u_Source, texCoord).xyz;
    texColor *= u_Exposure;

    //FragmentColor = U2Tonemapping(texColor);
    FragmentColor = RichardBurgessDawson(texColor);
    //FragmentColor = Reinhard(texColor);
    //FragmentColor = vec4(texColor, 1.0);
}

