#version 420

layout(location = 0) out vec3 o_Position;
layout(location = 1) out vec3 o_Normal;
layout(location = 2) out vec4 o_DiffuseSpec;


struct Input_Material
{
    vec3 Ke;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float illum;
    float d;
    float Ns;
    float sharpness;
    float Ni;
};


in VS_OUTPUT
{
	vec3 v_WorldPosition;
	vec3 v_Normal;
	vec2 v_TexCoords;
    vec3 v_Tangent;
	vec3 v_Bitangent;
} IN;

struct VS_CPY
{
	vec3 v_Normal;
} CPY;

uniform Input_Material IN_MATERIAL;

uniform bool u_map_Ka_bound;
uniform bool u_map_Kd_bound;
uniform bool u_map_Ks_bound;
uniform bool u_map_N_bound;
uniform sampler2D u_map_Ka;
uniform sampler2D u_map_Kd;
uniform sampler2D u_map_Ks;
uniform sampler2D u_map_N;


void main()
{
    Input_Material MATERIAL = IN_MATERIAL;
	
    CPY.v_Normal = normalize(IN.v_Normal);

    if (u_map_N_bound)
    {
        vec4 TEX_COLOR = texture(u_map_N, IN.v_TexCoords);
        mat3 normalSpace = mat3(normalize(IN.v_Tangent), normalize(IN.v_Bitangent), normalize(IN.v_Normal));
        
        //CPY.v_Normal = normalSpace * normalize((2 * TEX_COLOR.xyz) - vec3(1.0, 1.0, 1.0));
    }

    if (u_map_Kd_bound)
    {
        vec4 TEX_COLOR = texture(u_map_Kd, IN.v_TexCoords);
        MATERIAL.Kd *= TEX_COLOR.xyz;
    }

    o_Position = IN.v_WorldPosition;
    o_Normal = CPY.v_Normal;
    o_DiffuseSpec.xyz = MATERIAL.Kd.xyz;
}