#version 450 core

layout (location = 0) in vec3 aPosition;

uniform mat4 ProjViewMat;

void main()
{
    gl_Position = ProjViewMat * vec4(aPosition.xyz, 1.0);
}