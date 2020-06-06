#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ClipSpace;
out int instanceID;
out vec3 ToCameraVector;
out vec3 FromLightVector;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[10];
};

uniform vec3 cameraPosition;
uniform vec3 lightPosition;

const float tiling = 8.0;

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    vec4 worldPosition = model[instanceID] * vec4(aPos, 1.0);
    ClipSpace = projection * view * worldPosition;
    gl_Position = ClipSpace;
    TexCoords = aTexCoords * tiling;
    ToCameraVector = cameraPosition - worldPosition.xyz;
    FromLightVector = worldPosition.xyz - lightPosition;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ClipSpace;
in flat int instanceID;
in vec3 ToCameraVector;
in vec3 FromLightVector;


layout(std140, binding = 1) buffer textureLayers
{
    vec4 layer[10];
};

uniform sampler2DArray textureArray;

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D refractDepthTex;

uniform float moveFactor;
uniform vec3 lightColor;

const float waveStrength = 0.02;
const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main()
{
    //vec4 dudv = texture(textureArray, vec3(TexCoords, layer[instanceID].x)); //distortion
    //vec4 normal = texture(textureArray, vec3(TexCoords, layer[instanceID].y)); //normal
    
    vec2 ndc = (ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);

    //vec2 distortion = (texture(textureArray, vec3(TexCoords.x + moveFactor, TexCoords.y, layer[instanceID].x)).rg * 2.0 - 1.0) * waveStrength;
    //vec2 distortion2 = (texture(textureArray, vec3(-TexCoords.x + moveFactor, TexCoords.y + moveFactor, layer[instanceID].x)).rg * 2.0 - 1.0) * waveStrength;
    //distortion += distortion2;
    vec2 distortedTexCoords = texture(textureArray, vec3(TexCoords.x + moveFactor, TexCoords.y, layer[instanceID].x)).rg * 0.1;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 distortion = (texture(textureArray, vec3(distortedTexCoords, layer[instanceID].x)).rg * 2.0 - 1.0) * waveStrength;

    reflectTexCoords += distortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

    refractTexCoords += distortion;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    vec4 reflectColor = texture(reflectTex, reflectTexCoords);
    vec4 refractColor = texture(refractTex, refractTexCoords);

    vec3 viewVector = normalize(ToCameraVector);
    float reflectiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0)); //frsnel
    reflectiveFactor = pow(reflectiveFactor, 2.0);

    vec4 normalMap = texture(textureArray, vec3(distortedTexCoords, layer[instanceID].y));
    vec3 normal = vec3(normalMap.r * 2.0 - 1.0, normalMap.b, normalMap.g * 2.0 -1.0);
    normal = normalize(normal);

    vec3 reflectedLight = reflect(normalize(FromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity;

    FragColor = mix(reflectColor, refractColor, reflectiveFactor);
    FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);
}
