#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

struct ParticleData
{
	mat4 model;
	vec4 color;
};

layout(std430, binding = 0) buffer matrices
{
    mat4 proj;
    mat4 view;
};

layout(std430, binding = 1) buffer particle_data
{
    ParticleData particleData[500];
};

out vec2 TexCoords;
out vec4 ParticleColor;

void main()
{
    TexCoords = aTexCoords;
	ParticleColor = particleData[gl_InstanceID].color;
    gl_Position = proj * view * particleData[gl_InstanceID].model * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D textureName;

void main()
{
    vec4 texColor = texture(textureName, TexCoords);

	//if(texColor.a < 0.1)
       // discard;

    FragColor = texColor * ParticleColor;
}