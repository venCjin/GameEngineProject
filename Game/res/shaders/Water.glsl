#shader vertex
#version 400 core
layout (location = 0) in vec3 aPos;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoords = vec2(aPos.x/2.0 + 0.5, aPos.z/2.0 + 0.5);
}

#shader fragment
#version 400 core

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}