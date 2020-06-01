#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;
out int instanceID;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[10];
};

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;
    Normal = aNormal;

    gl_Position = projection * view * model[instanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in flat int instanceID;

layout(std140, binding = 1) buffer textureLayers
{
    vec4 layer[10];
};

uniform sampler2DArray textureArray;
uniform vec3 color;
uniform vec3 viewDir;
uniform float FresnelExponent;

void main()
{

    gl_FragDepth = 0.1;
    float fresnel = dot(Normal, normalize(-viewDir));
    fresnel = pow((1 - fresnel), FresnelExponent);
    FragColor = vec4(color * fresnel, fresnel);// * texture(textureArray, vec3(TexCoords, layer[instanceID].x));
}