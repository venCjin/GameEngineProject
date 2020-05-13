#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

out vec2 TexCoords;
out int instanceID;

layout(std430, binding = 2) buffer modelInstance
{
    mat4 view;
    mat4 projection;
    mat4 model[100];
};

struct BonesStruct
{
    mat4 bones[100];
};

layout(std430, binding = 4) buffer bones
{
    BonesStruct gBones[100];
};

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;

    mat4 BoneTransform = gBones[instanceID].bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[3]] * aWeights[3];

    vec4 PosL = BoneTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * model[instanceID] * PosL;

    TexCoords = aTexCoords;
    //gl_Position = projection * view * model[instanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in flat int instanceID;

uniform sampler2DArray textureArray;

layout(std430, binding = 3) buffer textureLayers
{
    vec4 layer[100];
};

void main()
{
    FragColor = texture(textureArray, vec3(TexCoords, layer[instanceID].x));
}