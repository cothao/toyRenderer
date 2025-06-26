#version 330 core

layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	
	vec4 viewModel = view * model * vec4(aPos, 1.);

	gl_Position = projection * viewModel;

	FragPos = vec3(viewModel);

	Normal = aNormal;

}