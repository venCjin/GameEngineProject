#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ClipSpace;
out int instanceID;

layout(std140, binding = 0) buffer matrixes
{
    mat4 view;
    mat4 projection;
    mat4 model[10];
};

const float tiling = 4.0;

void main()
{
    instanceID = gl_BaseInstance + gl_InstanceID;
    ClipSpace = projection * view * model[instanceID] * vec4(aPos, 1.0);
    gl_Position = ClipSpace;
    TexCoords = aTexCoords * tiling;
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ClipSpace;
in flat int instanceID;

layout(std140, binding = 1) buffer textureLayers
{
    vec4 layer[10];
};

uniform sampler2DArray textureArray;

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D refractDepthTex;

uniform float moveFactor;

const float waveStrenght = 0.02;

void main()
{
    //vec4 dudv = texture(textureArray, vec3(TexCoords, layer[instanceID].x)); //distortion
    //vec4 normal = texture(textureArray, vec3(TexCoords, layer[instanceID].y)); //normal
    
    vec2 ndc = (ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);

    vec2 distortion1 = (texture(textureArray, vec3(TexCoords.x + moveFactor, TexCoords.y, layer[instanceID].x)).rg * 2.0 - 1.0) * waveStrenght;
    vec2 distortion2 = (texture(textureArray, vec3(-TexCoords.x + moveFactor, TexCoords.y + moveFactor, layer[instanceID].x)).rg * 2.0 - 1.0) * waveStrenght;
    distortion1 += distortion2;

    reflectTexCoords += distortion1;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

    refractTexCoords += distortion1;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    vec4 reflectColor = texture(reflectTex, reflectTexCoords);
    vec4 refractColor = texture(refractTex, refractTexCoords);

    FragColor = mix(reflectColor, refractColor, 0.5f);//* vec4(0.0f, 0.0f, 1.0f, 1.0f);
    FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);//* vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
