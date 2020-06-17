#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out int instanceID;
out vec3 FragPos;
out vec3 Normal;
out vec3 bar;

uniform vec3 barOrientation;
uniform float barFill;


layout(std430, binding = 0) buffer matrixes
{
	mat4 view;
	mat4 projection;
	mat4 model[1000];
};

void main()
{
	instanceID = gl_BaseInstance + gl_InstanceID;
	vec3 v = (barOrientation * barFill);
	if (v.x == 0) v.x = 1;//aTexCoords.x;
	if (v.y == 0) v.y = 1;// -aTexCoords.y;
	bar = v;
	
	TexCoords = aTexCoords * vec2(v);

	mat4 activeView = view;

	//model[instanceID] = model[instanceID] * vec4(bar, 1.0f, 1.0f);
	gl_Position = projection * activeView * model[instanceID] * vec4(aPos, 1.0);
	FragPos = vec3(activeView * model[instanceID] * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(activeView * model[instanceID]))) * aNormal;

}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in flat int instanceID;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec3 bar;

uniform vec3 color;

layout(std430, binding = 1) buffer textureLayers
{
	vec4 layer[1000];
};

uniform sampler2DArray textureArray;






void main()
{
	

	vec4 c = texture(textureArray, vec3(TexCoords, layer[instanceID].x));
	FragColor = vec4(color, 1.0f) * c;//vec4(1.0, .0f, .0, .5);


}

