#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2DArray textureArray;
uniform float layer;

void main()
{
    FragColor = texture(textureArray, vec3(TexCoords, layer));
} 