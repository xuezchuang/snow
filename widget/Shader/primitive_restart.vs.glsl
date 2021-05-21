#version 330

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform mat3 test_normal_matrix;

layout (location = 0) in vec4 position;

out vec4 vs_fs_color;

void main(void)
{
    vec4 pos = vec4(position.x+gl_VertexID,position.yzw);
    gl_Position = projection_matrix * (model_matrix * pos);
    //test_normal_matrix = mat3(transpose(inverse(model_matrix));
}
