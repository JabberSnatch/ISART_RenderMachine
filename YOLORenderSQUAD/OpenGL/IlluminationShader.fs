#version 420

const vec4 defaultColor = vec4(1., 0.0, 0.0, 1.0);

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
	vec3 v_ViewDirection;
	vec3 v_LightDirection;
} IN;

uniform Input_Material IN_MATERIAL;
uniform Input_Light IN_LIGHT;

uniform bool u_map_Ka_bound;
uniform bool u_map_Kd_bound;
uniform bool u_map_Ks_bound;
uniform bool u_map_N_bound;
uniform sampler2D u_map_Ka;
uniform sampler2D u_map_Kd;
uniform sampler2D u_map_Ks;
uniform sampler2D u_map_N;

out vec4 FragmentColor;

void main(void)
{
    Input_Light LIGHT = IN_LIGHT;
    Input_Material MATERIAL = IN_MATERIAL;

    if (u_map_Ka_bound)
    {
        vec4 TEX_COLOR = texture(u_map_Ka, IN.v_TexCoords);
        MATERIAL.Ka *= TEX_COLOR.xyz;
    }

    if (u_map_Kd_bound)
    {
        vec4 TEX_COLOR = texture(u_map_Kd, IN.v_TexCoords);
        MATERIAL.Kd *= TEX_COLOR.xyz;
        MATERIAL.Ka *= TEX_COLOR.xyz;
    }

    if (u_map_Ks_bound)
    {
        vec4 TEX_COLOR = texture(u_map_Ks, IN.v_TexCoords);
        MATERIAL.Ks *= TEX_COLOR.xyz;
    }

    if (u_map_N_bound)
    {
        vec4 TEX_COLOR = texture(u_map_N, IN.v_TexCoords);
        //MATERIAL.Kd = TEX_COLOR.xyz;
        //IN.v_Normal += (2 * TEX_COLOR.xyz) - vec3(1.0, 1.0, 1.0);
    }

    LIGHT.Direction = normalize(LIGHT.Direction);
    
    IN.v_Normal = normalize(IN.v_Normal);
    IN.v_LightDirection = normalize(IN.v_LightDirection);
    IN.v_ViewDirection = normalize(IN.v_ViewDirection);

    vec3 ambient = LIGHT.Ia * MATERIAL.Ka;
    vec3 diffuse = LIGHT.Id * (max(dot(IN.v_LightDirection, IN.v_Normal), 0.0) * MATERIAL.Kd);

    vec3 specular;
    {
        //vec3 R = normalize(2 * max(dot(IN.v_LightDirection, IN.v_Normal), 0.0) * IN.v_Normal - IN.v_LightDirection);
        //specular = MATERIAL.Ks * LIGHT.Is * pow(max(dot(R, IN.v_ViewDirection), 0.0), MATERIAL.Ns);
    }
    {
        vec3 H = normalize((IN.v_LightDirection + IN.v_ViewDirection) / length(IN.v_LightDirection + IN.v_ViewDirection));
        specular = MATERIAL.Ks * LIGHT.Is * pow(max(dot(IN.v_Normal, H), 0.0), MATERIAL.Ns * 4);
    }
    
    specular = max(specular, 0.0);

    //gl_FragColor = vec4(abs(IN.v_Normal), 1.0);//ambient + diffuse + specular, 1.0);
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
