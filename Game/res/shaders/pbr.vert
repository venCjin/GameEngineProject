#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
    
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	
	Normal = mat3(transpose(inverse(view * model))) * aNormal;

    TexCoords = aTexCoord;
}