#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 pv;

void main()
{
    TexCoords = aTexCoords;
	WorldPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    Normal = mat3(aInstanceMatrix) * aNormal;

    gl_Position = pv * vec4(WorldPos, 1.0); 
}