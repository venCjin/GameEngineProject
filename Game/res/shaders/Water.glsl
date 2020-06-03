#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;

out vec2 TexCoords;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[10];
};

void main()
{
    gl_Position = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//texCoords = vec2(aPos.x/2.0 + 0.5, aPos.z/2.0 + 0.5);
}

#shader fragment
#version 460 core

//in vec2 texCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}