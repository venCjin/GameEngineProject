#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;

out vec4 ClipSpace;
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
    ClipSpace = projection * view * model[instanceID] * vec4(aPos, 1.0);
    gl_Position = ClipSpace;
}

#shader fragment
#version 460 core

out vec4 FragColor;
in vec4 ClipSpace;

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D refractDepthTex;

void main()
{
    //texture(textureArray, vec3(TexCoords, layer[instanceID].x)) //distortion
    //texture(textureArray, vec3(TexCoords, layer[instanceID].y)) //normal
    
    vec2 ndc  =(ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);

    vec4 reflectColor = texture(reflectTex, reflectTexCoords);
    vec4 refractColor = texture(refractTex, refractTexCoords);

    FragColor = mix(reflectColor, refractColor, 0.5f);//* vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
