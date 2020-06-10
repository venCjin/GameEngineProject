#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(std430, binding = 0) buffer matrices
{
    mat4 proj;
    mat4 view;
    mat4 model[500];
};

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = proj * view * model[gl_InstanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D textureName;

void main()
{
    vec4 texColor = texture(textureName, TexCoords);

	//if(texColor.a < 0.1)
       // discard;

    FragColor = texColor;
}