#version 410
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout (location = 0)in vec2 pos;
layout (location = 1)in vec4 color;

noperspective out vec4 finalColor;

void main()
{
  gl_Position = projection_matrix * model_matrix * vec4(pos, 0.0, 1.0);
  finalColor = color;
//      vec4 temp = vec4(0,0,0,1);
//    vec4 tt = projection_matrix * model_matrix * temp;
//    if( tt.x  < -0.812 && tt.y < 0.66)
//        finalColor = vec4(1.0,0.0,0.0,1.0);
//    else
//        finalColor = vec4(0.0,1.0,0.0,1.0);
}
