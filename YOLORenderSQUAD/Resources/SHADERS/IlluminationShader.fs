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

struct DirectionalLight
{
	vec3 Ia;
	vec3 Id;
	vec3 Is;
	vec3 Direction;
};
struct PointLight
{
	vec3 Ia;
	vec3 Id;
	vec3 Is;
	vec3 Position;
    float _pad;
    
    float Constant;
    float Linear;
    float Quadratic;
};
struct SpotLight
{
	vec3 Ia;
	vec3 Id;
	vec3 Is;
	vec3 Position;
	vec3 Direction;
    float _pad;

    float InnerCutoff;
	float OuterCutoff;
};

const unsigned int MAX_LIGHT_COUNT = 50;
//uniform unsigned int u_DirectionalCount;
//uniform DirectionalLight[MAX_LIGHT_COUNT] u_DirectionalLights;
//uniform unsigned int u_PointCount;
//uniform PointLight[MAX_LIGHT_COUNT] u_PointLights;
//uniform unsigned int u_SpotCount;
//uniform SpotLight[MAX_LIGHT_COUNT] u_SpotLights;

layout(std140, binding = 1)
uniform LightData
{
    unsigned int DirectionalCount;
    unsigned int PointCount;
    unsigned int SpotCount;

    DirectionalLight[MAX_LIGHT_COUNT] DirectionalLights;
    PointLight[MAX_LIGHT_COUNT] PointLights;
    SpotLight[MAX_LIGHT_COUNT] SpotLights;
} LIGHTS;

in VS_OUTPUT
{
    vec3 v_WorldPosition;
	vec3 v_Normal;
	vec4 v_Color;
	vec2 v_TexCoords;
	vec3 v_ViewDirection;
	vec3 v_LightDirection;
	vec3 v_Tangent;
	vec3 v_Bitangent;
} IN;

struct VS_CPY
{
	vec3 v_Normal;
	vec3 v_ViewDirection;
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

uniform bool u_skybox_bound;
uniform samplerCube u_skybox;

out vec4 FragmentColor;


vec3 ComputeAmbient(Input_Material MATERIAL)
{
    vec3 Ia = vec3(0.0);
    
    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
        Ia += LIGHTS.DirectionalLights[i].Ia;

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        float distance = length(light.Position - IN.v_WorldPosition);
        float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));
        Ia += light.Ia * attenuation;
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
        Ia += LIGHTS.SpotLights[i].Ia;

    vec3 ambient = Ia * MATERIAL.Ka;

    return ambient;
}

vec3 ComputeDiffuse(Input_Material MATERIAL, bool halfLambert)
{
    vec3 diffuse = vec3(0.0);

    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
    {
        DirectionalLight light = LIGHTS.DirectionalLights[i];
        light.Direction = -normalize(light.Direction);
        
        if (halfLambert)
            diffuse += light.Id * (max(pow((dot(light.Direction, CPY.v_Normal) + 0.5) * 0.5, 2), 0.0) * MATERIAL.Kd);
        else
            diffuse += light.Id * (max(dot(light.Direction, CPY.v_Normal), 0.0) * MATERIAL.Kd);
    }

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        vec3 direction = light.Position - IN.v_WorldPosition;
        float distance = length(direction);
        direction = normalize(direction);
        float intensity = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));
        
        if (halfLambert)
            diffuse += light.Id * intensity * (max(pow((dot(direction, CPY.v_Normal) + 0.5) * 0.5, 2), 0.0) * MATERIAL.Kd);
        else
            diffuse += light.Id * intensity * (max(dot(direction, CPY.v_Normal), 0.0) * MATERIAL.Kd);
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
    {
        SpotLight light = LIGHTS.SpotLights[i];
        vec3 direction = light.Position - IN.v_WorldPosition;
        direction = normalize(direction);
        float theta = dot(direction, -normalize(light.Direction));
        float bufferTheta = dot(direction, -normalize(LIGHTS.SpotLights[i].Direction));
        float epsilon = light.InnerCutoff - light.OuterCutoff;
        float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);
        
        if (theta >= light.OuterCutoff)
        {
            if (halfLambert)
                diffuse += light.Id * intensity * (max(pow((dot(direction, CPY.v_Normal) + 0.5) * 0.5, 2), 0.0) * MATERIAL.Kd);
            else
                diffuse += light.Id * intensity * (max(dot(direction, CPY.v_Normal), 0.0) * MATERIAL.Kd);
        }
    }

    return diffuse;
}

vec3 ComputeSpecular(Input_Material MATERIAL, bool blinnPhong)
{
    vec3 specular = vec3(0.0);

    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
    {
        DirectionalLight light = LIGHTS.DirectionalLights[i];
        light.Direction = -normalize(light.Direction);
        
        if (blinnPhong)
        {
            vec3 H = normalize((light.Direction + CPY.v_ViewDirection) / length(light.Direction + CPY.v_ViewDirection));
            specular += MATERIAL.Ks * light.Is * pow(max(dot(CPY.v_Normal, H), 0.0), MATERIAL.Ns * 4);
        }
        else
        {
            vec3 R = normalize(2 * max(dot(light.Direction, CPY.v_Normal), 0.0) * CPY.v_Normal - light.Direction);
            specular += MATERIAL.Ks * light.Is * pow(max(dot(R, CPY.v_ViewDirection), 0.0), MATERIAL.Ns);
        }
    }

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        vec3 direction = light.Position - IN.v_WorldPosition;
        float distance = length(direction);
        direction = normalize(direction);
        float intensity = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));

        if (blinnPhong)
        {
            vec3 H = normalize((direction + CPY.v_ViewDirection) / length(direction + CPY.v_ViewDirection));
            specular += MATERIAL.Ks * light.Is * intensity * pow(max(dot(CPY.v_Normal, H), 0.0), MATERIAL.Ns * 4);
        }
        else
        {
            vec3 R = normalize(2 * max(dot(direction, CPY.v_Normal), 0.0) * CPY.v_Normal - direction);
            specular += MATERIAL.Ks * light.Is * intensity * pow(max(dot(R, CPY.v_ViewDirection), 0.0), MATERIAL.Ns);
        }
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
    {
        SpotLight light = LIGHTS.SpotLights[i];
        vec3 direction = light.Position - IN.v_WorldPosition;
        direction = normalize(direction);
        float theta = dot(direction, -normalize(light.Direction));
        float epsilon = light.InnerCutoff - light.OuterCutoff;
        float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);
    
        if (theta >= light.OuterCutoff)
        {
            if (blinnPhong)
            {
                vec3 H = normalize((direction + CPY.v_ViewDirection) / length(direction + CPY.v_ViewDirection));
                specular += MATERIAL.Ks * light.Is * intensity * pow(max(dot(CPY.v_Normal, H), 0.0), MATERIAL.Ns * 4);
            }
            else
            {
                vec3 R = normalize(2 * max(dot(direction, CPY.v_Normal), 0.0) * CPY.v_Normal - direction);
                specular += MATERIAL.Ks * light.Is * intensity * pow(max(dot(R, CPY.v_ViewDirection), 0.0), MATERIAL.Ns);
            }
        }
    }

    
    specular = max(specular, 0.0);
    return specular;
}

void main(void)
{
    //vec4 halfPosition = fract(gl_FragCoord * 0.5);
    //if ((halfPosition.x < 0.5 && halfPosition.y > 0.5) 
//  //      || (halfPosition.x > 0.5 && halfPosition.y < 0.5))
    //    )
    //    discard;

    Input_Material MATERIAL = IN_MATERIAL;
	
    CPY.v_Normal = IN.v_Normal;
	CPY.v_ViewDirection = IN.v_ViewDirection;

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

    vec3 BT;
    if (u_map_N_bound)
    {
        vec4 TEX_COLOR = texture(u_map_N, IN.v_TexCoords);
        vec3 bitangent = cross(IN.v_Normal, IN.v_Tangent);
        mat3 normalSpace = mat3(normalize(IN.v_Tangent), normalize(IN.v_Bitangent), normalize(IN.v_Normal));
        
        //CPY.v_Normal = normalSpace * normalize((2 * TEX_COLOR.xyz) - vec3(1.0, 1.0, 1.0));

        BT = bitangent;
    }

	CPY.v_Normal = normalize(CPY.v_Normal);
    CPY.v_ViewDirection = normalize(CPY.v_ViewDirection);


    vec3 ambient = ComputeAmbient(MATERIAL);
    vec3 diffuse = ComputeDiffuse(MATERIAL, false);
    vec3 specular = ComputeSpecular(MATERIAL, true);

    vec3 linearColor = ambient + diffuse * 3 + specular;
    if (u_skybox_bound)
    {
        vec3 R = reflect(-CPY.v_ViewDirection, CPY.v_Normal);
        //linearColor = texture(u_skybox, R).xyz / 10.0;
    }

    //gl_FragColor = vec4(pow(linearColor, vec3(1.0 / 2.2)), 1.0);
    //gl_FragColor = vec4(pow(abs(CPY.v_Normal), vec3(2.2)), 1.0);
    gl_FragColor = vec4(vec3(MATERIAL.Ns), 1.0);
    //gl_FragColor = vec4(linearColor, 1.0);
    //gl_FragColor = vec4(1.0, 0, 0, 1.0);
    //gl_FragColor = vec4(u_PointLights[0].Constant - LIGHTS.PointLights[0].Constant, 0.0, 0.0, 1.0);
}
