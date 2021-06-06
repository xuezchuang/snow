#version 330

#define MAX_PARAM 12

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
uniform vec4 parameters[MAX_PARAM];

#define recti parameters[gl_InstanceID * MAX_PARAM + 0]
#define rect parameters[gl_InstanceID * MAX_PARAM + 1]

flat out float lineWidth;

vec2 do_widget(void)
{
  lineWidth = abs(rect.x - recti.x);
  vec2 emboss_ofs = vec2(0.0, -lineWidth);
  vec2 v_pos[4] = vec2[4](rect.xz + emboss_ofs, rect.xw, rect.yz + emboss_ofs, rect.yw);
  vec2 pos = v_pos[gl_VertexID];
  return pos;
}

void main()
{
  vec2 pos = do_widget();

  gl_Position = vec4(pos, 0.0, 1.0);
}
