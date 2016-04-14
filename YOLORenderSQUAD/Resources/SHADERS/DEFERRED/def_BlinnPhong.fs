#version 420

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


uniform vec3 u_ViewPosition;

in vec2 texCoord;
uniform sampler2D u_PositionMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_DiffuseSpecMap;
layout(location = 0) out vec3 FragmentColor;

vec3 WorldPosition;
vec3 Normal;
vec3 Albedo;
float SpecularIntensity;
vec3 ViewDirection;


vec3 ComputeAmbient()
{
    vec3 Ia = vec3(0.0);
    
    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
        Ia += LIGHTS.DirectionalLights[i].Ia;

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        float distance = length(light.Position - WorldPosition);
        float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));
        Ia += light.Ia * attenuation;
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
        Ia += LIGHTS.SpotLights[i].Ia;

    vec3 ambient = Ia * Albedo;

    return ambient;
}


vec3 ComputeDiffuse()
{
    vec3 diffuse = vec3(0.0);

    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
    {
        DirectionalLight light = LIGHTS.DirectionalLights[i];
        light.Direction = -normalize(light.Direction);
        
        diffuse += light.Id * (max(dot(light.Direction, Normal), 0.0) * Albedo);
    }

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        vec3 direction = light.Position - WorldPosition;
        float distance = length(direction);
        direction = normalize(direction);
        float intensity = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));
        
        diffuse += light.Id * intensity * (max(dot(direction, Normal), 0.0) * Albedo);
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
    {
        SpotLight light = LIGHTS.SpotLights[i];
        vec3 direction = light.Position - WorldPosition;
        direction = normalize(direction);
        float theta = dot(direction, -normalize(light.Direction));
        float bufferTheta = dot(direction, -normalize(LIGHTS.SpotLights[i].Direction));
        float epsilon = light.InnerCutoff - light.OuterCutoff;
        float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);
        
        if (theta >= light.OuterCutoff)
        {
            diffuse += light.Id * intensity * (max(dot(direction, Normal), 0.0) * Albedo);
        }
    }

    return diffuse;
}


vec3 ComputeSpecular()
{
    vec3 specular = vec3(0.0);

    for(unsigned int i = 0; i < LIGHTS.DirectionalCount; ++i)
    {
        DirectionalLight light = LIGHTS.DirectionalLights[i];
        light.Direction = -normalize(light.Direction);
        
        vec3 H = normalize(light.Direction + ViewDirection);// / length(light.Direction + ViewDirection));
        specular += Albedo * light.Is * pow(max(dot(Normal, H), 0.0), SpecularIntensity * 4) * max(dot(Normal, light.Direction), 0.0);
    }

    for(unsigned int i = 0; i < LIGHTS.PointCount; ++i)
    {
        PointLight light = LIGHTS.PointLights[i];
        vec3 direction = light.Position - WorldPosition;
        float distance = length(direction);
        direction = normalize(direction);
        float intensity = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * pow(distance, 2));

        vec3 H = normalize(direction + ViewDirection);// / length(direction + ViewDirection));
        specular += Albedo * light.Is * intensity * pow(max(dot(Normal, H), 0.0), SpecularIntensity * 4);
    }

    for(unsigned int i = 0; i < LIGHTS.SpotCount; ++i)
    {
        SpotLight light = LIGHTS.SpotLights[i];
        vec3 direction = light.Position - WorldPosition;
        direction = normalize(direction);
        float theta = dot(direction, -normalize(light.Direction));
        float epsilon = light.InnerCutoff - light.OuterCutoff;
        float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);
    
        if (theta >= light.OuterCutoff)
        {
            vec3 H = normalize(direction + ViewDirection);// / length(direction + ViewDirection));
            specular += Albedo * light.Is * intensity * pow(max(dot(Normal, H), 0.0), SpecularIntensity * 4);
        }
    }

    specular = max(specular, 0.0);
    return specular;
}


void main()
{
    WorldPosition = texture(u_PositionMap, texCoord).xyz;
    Normal = normalize(texture(u_NormalMap, texCoord).xyz);
    Albedo = texture(u_DiffuseSpecMap, texCoord).xyz;
    SpecularIntensity = texture(u_DiffuseSpecMap, texCoord).w;
    ViewDirection = normalize(u_ViewPosition - WorldPosition);


    vec3 ambient = ComputeAmbient();
    vec3 diffuse = ComputeDiffuse();
    vec3 specular = ComputeSpecular();
   

    FragmentColor = ambient + diffuse + specular;
    //FragmentColor = specular;
    //FragmentColor = vec4(vec3(SpecularIntensity), 1.0);
}