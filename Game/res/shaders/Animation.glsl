#shader vertex
#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[100];

void main()
{
	mat4 BoneTransform	=  gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform		+= gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform		+= gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform		+= gBones[aBoneIDs[3]] * aWeights[3];

	vec4 PosL = BoneTransform * vec4(aPosition, 1.0);

    gl_Position = projection * view * model * PosL;
	TexCoords = aTexCoord;
}

#shader fragment
#version 460 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D textureArray;

void main()
{
    FragColor = texture(textureArray, TexCoords);
} 