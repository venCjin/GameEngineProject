#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;

layout(std430, binding = 6) buffer matrixes
{
    mat4 model[2000];
};

void main()
{
    gl_Position = lightSpaceMatrix * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}