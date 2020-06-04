#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

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
    gl_Position = projection * view * model[instanceID] * vec4(aPos, 1.0);

    //texCoords = vec2(aPos.x/2.0 + 0.5, aPos.z/2.0 + 0.5);
    TexCoords = aTexCoords;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D reflectTex;
uniform sampler2D refractTex;

void main()
{
    //texture(textureArray, vec3(TexCoords, layer[instanceID].x)) //distortion
    
    //texture(textureArray, vec3(TexCoords, layer[instanceID].y)) //normal

    FragColor =  vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
