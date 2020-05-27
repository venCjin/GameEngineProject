#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out int instanceID;

layout(std430, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[40000];
};

void main()
{   
    instanceID = gl_BaseInstance + gl_InstanceID;
    TexCoords = aTexCoords;
    gl_Position = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in flat int instanceID;

layout(std430, binding = 1) buffer textureLayers
{
    vec4 layer[40000];
};

uniform sampler2DArray textureArray;

void main()
{
    
    gl_FragDepth = .1f;
    //float alpha = ;

    FragColor = vec4(1.0f, 1.f, 1.f, .5f) * texture(textureArray, vec3(TexCoords, layer[instanceID].x));
} 