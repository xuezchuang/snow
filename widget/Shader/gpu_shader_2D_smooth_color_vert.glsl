#version 330
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout (location = 0)in vec2 pos;
layout (location = 1)in vec4 color;

out vec4 finalColor;

void main()
{
  gl_Position = projection_matrix * model_matrix * vec4(pos, 0.0, 1.0);
  finalColor = color;
}
