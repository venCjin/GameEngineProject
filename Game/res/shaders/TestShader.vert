#version 460 core

layout (location = 0) in vec3 aPosition;

layout(std430, binding = 0) buffer modelInstance
{
    mat4 model[1000];
};

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPosition.xyz, 1.0);
}