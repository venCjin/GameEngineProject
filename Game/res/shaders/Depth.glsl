#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 lightSpaceMatrix;

layout(std430, binding = 0) buffer matrixes
{
    mat4 a;
    mat4 b;
    mat4 model[2000];
};

out int instanceID;
out vec2 TexCoords;

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;
    gl_Position = lightSpaceMatrix * model[instanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

layout(std430, binding = 1) buffer layers
{
    vec4 layer[2000];
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