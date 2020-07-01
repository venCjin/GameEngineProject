#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[100];
};

void main()
{
    Normal = aNormal;
    gl_Position = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 Normal;

uniform vec3 color;
uniform vec3 viewDir;
uniform float FresnelExponent;

void main()
{
    gl_FragDepth = .1f;
    float fresnel = dot( Normal, normalize(viewDir) );
    fresnel = pow( (1 - fresnel), FresnelExponent );
    FragColor = vec4( color * fresnel, fresnel );
} 