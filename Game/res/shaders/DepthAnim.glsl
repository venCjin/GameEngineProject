#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

uniform mat4 lightSpaceMatrix;

layout(std140, binding = 0) buffer matrixes
{
    mat4 a;
    mat4 b;
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

out int instanceID;
out vec2 TexCoords;

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;

    mat4 BoneTransform = gBones[instanceID].bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform += gBones[instanceID].bones[aBoneIDs[3]] * aWeights[3];

    vec4 PosL = BoneTransform * vec4(aPos, 1.0);
    gl_Position = lightSpaceMatrix * model[gl_BaseInstance + gl_InstanceID] * PosL;
}

#shader fragment
#version 460 core

layout(std140, binding = 2) buffer layers
{
    vec4 layer[100];
};

in flat int instanceID;
in vec2 TexCoords;

out vec4 color;

uniform sampler2DArray textureArray;

void main()
{
    float alpha = texture(textureArray, vec3(TexCoords, layer[instanceID].x)).a;
    if (alpha < 0.5)
    {
        discard;
    }

    color = vec4(1.0);
}