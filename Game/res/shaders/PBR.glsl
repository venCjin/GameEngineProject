#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out int instanceID;
out vec3 FragPos;
out vec3 Normal;

layout(std430, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[40000];
};

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;

    gl_Position = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0);    
	FragPos = vec3(view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0));	
	Normal = mat3(transpose(inverse(view * model[gl_BaseInstance + gl_InstanceID]))) * aNormal;

}

#shader fragment
#version 460 core

const float PI = 3.14159265359;

struct DirectionalLight
{
	vec3 position;	// needed for shadow mapping // should position be calculated in View Space? it's onlt used for shadow mapping
	vec3 direction; // direction probably needs to be calculated in View Space - in CPU as well
	vec3 color;
};

struct PointLight
{
	vec3 position; // position needs to be calculated in View Space - in CPU
	vec3 color;
};

struct SpotLight
{
	vec3 position; // position needs to be calculated in View Space - in CPU
	vec3 direction;
	vec3 color;

	float cutOff;
	float outerCutOff;
};

#define NR_DIRLIGHTS 1
#define NR_POINTLIGHTS 1
#define NR_SPOTLIGHTS 1

out vec4 FragColor;

in vec2 TexCoords;
in flat int instanceID;
in vec3 FragPos;
in vec3 Normal;

layout(std430, binding = 1) buffer textureLayers
{
    vec4 layer[40000];
};

uniform sampler2DArray textureArray;

layout(std430, binding = 5) buffer lightData
{    
	float ao;
	float metallic;
	float roughness;

	//vec3 position;	// needed for shadow mapping // should position be calculated in View Space? it's onlt used for shadow mapping
	//vec3 direction; // direction probably needs to be calculated in View Space - in CPU as well
	//vec3 color1;

	DirectionalLight dirLight;
};



// Material parameters
//uniform float metallic;
//uniform float roughness;
//uniform float ao;

// Lights
//uniform DirectionalLight dirLights[NR_DIRLIGHTS];
uniform PointLight pointLights[NR_POINTLIGHTS];
uniform SpotLight spotLights[NR_SPOTLIGHTS];

// Light Types Functions
vec3 CalcDirLight(DirectionalLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo);
vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo);
vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo);

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

void main()
{
	vec3 textureColor = texture(textureArray, vec3(TexCoords, layer[instanceID].x)).rgb;

	vec3 N = normalize(Normal);
	vec3 V = normalize(-FragPos);	// Calculated in View Space
	vec3 albedo = pow(textureColor, vec3(2.2));

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	Lo += CalcDirLight(dirLight, N, V, F0, albedo);

	//for (int i = 0; i < NR_DIRLIGHTS; i++)
		//Lo += CalcDirLight(dirLights[i], N, V, F0, albedo);
	//for (int i = 0; i < NR_POINTLIGHTS; i++)
		//Lo += CalcPointLight(pointLights[i], N, V, FragPos, F0, albedo);
	//for (int i = 0; i < NR_SPOTLIGHTS; i++)
		//Lo += CalcSpotLight(spotLights[i], N, V, FragPos, F0, albedo);

	// should we multiply specular by kS? According to the formula - yes, according to the learnopengl code - no (he says that we already multiplied BRDF by kS (F), but it's not correct according to the formula)
	
	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;

	// HDR
	color = color / (color + vec3(1.0));
	// Gamma
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);

} 

vec3 CalcDirLight(DirectionalLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo)
{
	vec3 L = normalize(-light.direction.xyz);
	vec3 H = normalize(V + L);
	
	vec3 radiance = light.color.rgb;	

	// cook-torrance brdf
	float NDF	= DistributionGGX(N, H, roughness);
	float G		= GeometrySmith(N, V, L, roughness);
	vec3 F		= FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator		= NDF * G * F;
	float denominator	= 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular		= numerator / max(denominator, 0.001);

	// calculating outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo)
{
	float distance = length(light.position - fragPos);

	vec3 L = normalize(light.position - fragPos);
	vec3 H = normalize(V + L);

	float attenuation	= 1.0 / (distance * distance);	// change when gamma correction added
	vec3 radiance		= light.color * attenuation; // consider multiplying this by 255, it might be too dark

	// cook-torrance brdf
	float NDF	= DistributionGGX(N, H, roughness);
	float G		= GeometrySmith(N, V, L, roughness);
	vec3 F		= FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator		= NDF * G * F;
	float denominator	= 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular		= numerator / max(denominator, 0.001);

	// calculating outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL;	
}

vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo)
{
	float distance = length(light.position - fragPos);	
	
	vec3 L = normalize(light.position - fragPos);
	vec3 H = normalize(V + L);

	float attenuation	= 1.0 / (distance * distance);	// change when gamma correction added
	vec3 radiance		= light.color  * 255.0 * attenuation; // we need to multiply color by 255.0 to reach proper intensity

	// cook-torrance brdf
	float NDF	= DistributionGGX(N, H, roughness);
	float G		= GeometrySmith(N, V, L, roughness);
	vec3 F		= FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator		= NDF * G * F;
	float denominator	= 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular		= numerator / max(denominator, 0.001);

	// SpotLight intensity
	float theta = dot(L, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// calculating outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL * intensity;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num /denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num	= NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1	= GeometrySchlickGGX(NdotV, roughness);
	float ggx2	= GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}