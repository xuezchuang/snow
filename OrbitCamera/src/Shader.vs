//顶点着色器代码
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 color;
out vec3 FragPos;
out vec3 Normal;
out vec4 glcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out mat4 model_matrix;
//out mat3 tt;

void main()
{
	gl_Position = projection * view * model * vec4 (aPos, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;
	model_matrix = model;
	glcolor = color;
	//model_matrix = model;
	//mat4 tetmp = model;
	//model = mat4();
	//model = tetmp;
}
