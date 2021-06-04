#version 330

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;

out vec4 vs_fs_color;

void main(void)
{
    vec4 pos = vec4(position.xy,0,1);
    gl_Position = projection_matrix * (model_matrix * pos);
    vs_fs_color = color;
}
