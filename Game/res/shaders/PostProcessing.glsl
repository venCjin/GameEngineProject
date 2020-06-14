#shader vertex
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform float time;

void main()
{
    TexCoords = aTexCoords;

    gl_Position = vec4(aPos, -0.1, 1.0);

    float strength = 0.01;
    gl_Position.x += cos(time * 10) * strength;
    gl_Position.y += cos(time * 15) * strength;
}

#shader fragment
#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform vec2 offsets[9];
uniform float blur_kernel[9];

void main()
{
    FragColor = vec4(0.0);

    vec3 sampleD[9];

    for (int i = 0; i < 9; i++)
        sampleD[i] = vec3(texture(image, TexCoords + offsets[i]));

    for (int i = 0; i < 9; i++)
        FragColor += vec4(sampleD[i] * blur_kernel[i], 0.0f);

    FragColor.a = 1.0f;
}