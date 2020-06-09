#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;
out int instanceID;
out vec4 FragPos;

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
    vec4 tmp = projection * view * model[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0);
    FragPos = tmp;
    gl_Position = tmp;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPos;
in flat int instanceID;

layout(std140, binding = 1) buffer textureLayers
{
    vec4 layer[10];
};

uniform sampler2DArray textureArray;
uniform vec3 color;
uniform vec3 viewDir;
uniform float FresnelExponent;
//uniform int OnSurface;
void main()
{
    float ndc_depth = FragPos.z / FragPos.w;
    ndc_depth = 0.5 * ndc_depth + 0.5;
    gl_FragDepth = .1f;//*(1-OnSurface) + ndc_depth * OnSurface;
    float fresnel = dot(Normal, -normalize(viewDir));
    fresnel = pow((1 - fresnel), FresnelExponent);
    vec4 outputcolor = /*OnSurface * texture(textureArray, vec3(TexCoords, layer[instanceID].x)) +*/ vec4(/*(1-OnSurface) */ color * fresnel, fresnel);

    FragColor = outputcolor;// * texture(textureArray, vec3(TexCoords, layer[instanceID].x));
}