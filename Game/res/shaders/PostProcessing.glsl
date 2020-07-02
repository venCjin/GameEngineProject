#shader vertex
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform float time;
uniform bool shakeEnabled;

void main()
{
    TexCoords = aTexCoords;

    gl_Position = vec4(aPos, -0.1, 1.0);

    if (shakeEnabled)
    {
        float strength = 0.01;
        gl_Position.x += cos(time * 10) * strength;
        gl_Position.y += cos(time * 15) * strength;
    }
}

#shader fragment
#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform vec2 offsets[9];
uniform float blur_kernel[9];
uniform bool blurEnabled;

uniform vec3 resolution;
uniform float Falloff;

void main()
{
    FragColor = vec4(0.0);

    if (blurEnabled)
    {
        vec3 sampleD[9];

        for (int i = 0; i < 9; i++)
            sampleD[i] = vec3(texture(image, TexCoords + offsets[i]));

        for (int i = 0; i < 9; i++)
            FragColor += vec4(sampleD[i] * blur_kernel[i], 0.0f);

        vec2 uv = gl_FragCoord.xy / resolution.xy;
        vec2 coord = (uv - 0.5) * (resolution.x / resolution.y) * 2.0;
        float rf = sqrt(dot(coord, coord)) * Falloff;
        float rf2_1 = rf * rf + 1.0;
        float e = 1.0 / (rf2_1 * rf2_1);

        //FragColor = vec4(FragColor.rgb * e, 1.0);
        FragColor = vec4(FragColor.rgb * e, 1.0);
    }
    else
    {
        FragColor = texture(image, TexCoords);
    }
}