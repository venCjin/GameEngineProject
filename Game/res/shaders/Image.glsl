#shader vertex
#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, -0.1, 1.0);
    TexCoords = aTexCoords;
}

#shader fragment
#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D fontTexture;

void main()
{
    FragColor = texture(fontTexture, TexCoords);
}