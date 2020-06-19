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

const float tiling = 3.0;

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
const float shineDamper = 15.0;
const float reflectivity = 0.6;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
