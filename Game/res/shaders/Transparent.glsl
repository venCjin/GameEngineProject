#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;


out vec3 Normal;
out vec2 TexCoords;
out int instanceID;


layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[100];
};

struct BonesStruct
{
    mat4 bones[100];
};

layout(std430, binding = 1) buffer bones
{
    BonesStruct gBones[100];
};
void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;

    mat4 BoneTransform = gBones[instanceID].bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[3]] * aWeights[3];

    vec4 PosL = BoneTransform * vec4(aPos, 1.0);
    Normal = aNormal;
    gl_Position = projection * view * model[instanceID] * PosL;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

in flat int instanceID;



uniform sampler2DArray textureArray;
uniform vec3 color;
uniform vec3 viewDir;
uniform float FresnelExponent;
//uniform int OnSurface;
void main()
{
    gl_FragDepth = .1f;
    float fresnel = dot (Normal, normalize(viewDir));
    fresnel = pow((1 - fresnel), FresnelExponent);
    vec4 outputcolor =  vec4( color * fresnel, fresnel);
    FragColor = outputcolor;
} 