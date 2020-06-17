#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out int instanceID;
out vec3 FragPos;
out vec3 FragPos2;
out vec3 Normal;
out vec4 FragPosLightSpace1;
out vec3 TangentFragPos;
out vec3 TangentFragPos2;
out mat3 TBN;
out mat3 TBN2;
out mat4 view2;

uniform mat4 lightSpaceMatrix1;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[12];
};

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;

	view2 = view;

    gl_Position = projection * view * model[instanceID] * vec4(aPos, 1.0);    
	FragPos = vec3(view * model[instanceID] * vec4(aPos, 1.0));	
	
	//S jescze raz frag pos bez view     
	FragPos2 = vec3(model[instanceID] * vec4(aPos, 1.0));	
	
	Normal = mat3(transpose(inverse(view * model[instanceID]))) * aNormal;

	FragPosLightSpace1 = lightSpaceMatrix1 * vec4(vec3(model[instanceID] * vec4(aPos, 1.0)), 1.0);

	// TBN
	mat3 normalMatrix = mat3(transpose(inverse(view * model[instanceID])));
	vec3 T = normalize(normalMatrix* aTangent);
	vec3 N = normalize(normalMatrix* aNormal);
	vec3 B = normalize(normalMatrix * aBitangent);
	//T = normalize(T - dot(T, N) * N);
	//vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	// S: druga macierz tbn bez bez view   
	normalMatrix = mat3(transpose(inverse(view * model[instanceID])));
	T = normalize(normalMatrix * aTangent);
	N = normalize(normalMatrix * aNormal);
	B = normalize(normalMatrix * aBitangent);
	TBN2 = mat3(T, B, N);

	//

	TangentFragPos = TBN * FragPos;
	TangentFragPos2 = TBN2 * FragPos2;
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
in vec3 FragPos2;
in vec3 Normal;
in vec4 FragPosLightSpace1;
in vec3 TangentFragPos;
in vec3 TangentFragPos2;
in mat3 TBN;
in mat3 TBN2;
in mat4 view2;

uniform sampler2D shadowMap1;

layout(std140, binding = 1) buffer textureLayers
{
    vec4 layer[10];
};

uniform sampler2DArray textureArray;

layout(std140, binding = 2) buffer lightData
{    
	float ao;
	float metallic;
	float roughness;

	DirectionalLight dirLight;
};


// Lights
//uniform DirectionalLight dirLights[NR_DIRLIGHTS];
uniform PointLight pointLights[NR_POINTLIGHTS];
uniform SpotLight spotLights[NR_SPOTLIGHTS];

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float heightScale);
vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir, float heightScale);

// Light Types Functions
vec3 CalcDirLight(DirectionalLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo);
vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo);
vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 fragPos, vec3 F0, vec3 albedo);

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPos, sampler2D shadowMap, vec3 N);

void main()
{
	vec3 tangentV = normalize(-TangentFragPos);
	vec3 tangentV2 = normalize(-TangentFragPos2);
	vec2 texCoords = TexCoords;
	
	// checkin if material has height map
	if (layer[instanceID].w < 1.0)
	{
		texCoords = TexCoords;
	}
	else 
	{
		// wywojuemy metode paralaxmapping z innymi argumentami     
		// (teraz jest w view space) W WORLD SPACE -> viewDir = (frag - camera) * TBN_World    
		
		vec3 cam = vec3(view2[3][0], view2[3][1], view2[3][2]);
		vec3 xxx = normalize(FragPos2 - cam) * TBN;
		texCoords = ParallaxMapping2(TexCoords, xxx, 0.05f);

		if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
			discard;
	}

	
	vec4 tex = texture(textureArray, vec3(texCoords, layer[instanceID].x));

	if (tex.a < 0.2)
		discard;

	vec3 textureColor = tex.rgb;

	vec3 normal = texture(textureArray, vec3(texCoords, layer[instanceID].y)).rgb;
	float met = texture(textureArray, vec3(texCoords, layer[instanceID].z)).r;
	
	normal = normalize(normal * 2.0 - 1.0);
	vec3 N = normalize(TBN * normal);

	// checkin if material has normal map
	if (layer[instanceID].y < 1.0)
		N = normalize(Normal);

	// checkin if material has metallic map
	if (layer[instanceID].z < 1.0)
		met = metallic;

	vec3 V = normalize(-FragPos);	// View vector calculated in View Space
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
	//FragColor = vec4(ao, metallic, roughness, 1.0);
	//FragColor = vec4(dirLight.color, 1.0);
	
} 

// viewDir powinno w worldspace
// druga macierz tbn ktora nie bierze pod uwage view + fragment ktory tez nie bedzie w view
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float heightScale)
{
	float height = texture(textureArray, vec3(texCoords, layer[instanceID].w)).r;
	return texCoords - viewDir.xy / viewDir.z * (height * heightScale);

	//float height =  texture(depthMap, texCoords).r;    
    //vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    //return texCoords - p;    
}

vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir, float heightScale)
{
	// number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

	// get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(textureArray, vec3(currentTexCoords, layer[instanceID].w)).r;

	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(textureArray, vec3(currentTexCoords, layer[instanceID].w)).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

	// get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(textureArray, vec3(prevTexCoords, layer[instanceID].w)).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
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

	/// SHADOW MAPPING
	float shadow = ShadowCalculation(FragPosLightSpace1, light.position, shadowMap1, N);
	return (1.0 - shadow) * (kD * albedo / PI + specular) * radiance * NdotL; // WITH SHADOWS
	//return (kD * albedo / PI + specular) * radiance * NdotL;
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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPos, sampler2D shadowMap, vec3 N)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; 
	float closestDepth = texture(shadowMap, projCoords.xy).r;   
	float currentDepth = projCoords.z;  
	vec3 lightDir = normalize(lightPos - FragPos);
	float bias = max(0.05 * (1.0 - dot(N, lightDir)), 0.005);
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0.0;

	if(projCoords.z <= 1.0)
	{
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
			}    
		}
		shadow /= 9.0;
	}

	return shadow;
}
