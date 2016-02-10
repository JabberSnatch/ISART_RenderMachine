#version 430

const vec4 defaultColor = vec4(1., 0.0, 0.0, 1.0);

struct Input_Material
{
    vec3 Ke;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
};

struct Input_Light
{
    vec4 Position;
    vec3 Direction;
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};

in VS_OUTPUT
{
	vec3 v_Normal;
	vec4 v_Color;
	vec2 v_TexCoords;
} IN;

uniform Input_Material IN_MATERIAL;
uniform Input_Light IN_LIGHT;
uniform sampler2D u_TextureUnit;

out vec4 FragmentColor;

void main(void)
{
    Input_Light LIGHT = IN_LIGHT;
    Input_Material MATERIAL = IN_MATERIAL;

    vec4 TEX_COLOR = texture(u_TextureUnit, IN.v_TexCoords);
    MATERIAL.Ka = TEX_COLOR.xyz;

    LIGHT.Direction = normalize(LIGHT.Direction);
    
    vec3 ambiant = LIGHT.Ia * MATERIAL.Ka;
    vec3 diffuse = LIGHT.Id * (max(dot(IN.v_Normal, LIGHT.Direction), 0.0) * MATERIAL.Kd);

    FragmentColor = vec4(ambiant + diffuse, 1.0);
}
