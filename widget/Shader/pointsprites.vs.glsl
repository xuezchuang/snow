#version 330

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

in vec4 position;
in vec4 vColor1;

out vec4 bcd;
void main(void)
{
    vec4 pos = projection_matrix * (model_matrix * position);
    //gl_PointSize = (1.0 - pos.z / pos.w) * 64.0;
    gl_Position = position;
    bcd = vColor1;
}

//void main(void)
//
//   const vec4 color1 = vec4(0.6, 0.0, 0.0, 1.0);
//   const vec4 color2 = vec4(0.9, 0.7, 1.0, 0.0);
//
//   vec2 temp = gl_PointCoord - vec2(0.5,0.5);
//   float f = dot(temp, temp);
//
//   if (f > 0.25)
//      discard;
//
//   color = mix(color1, color2, smoothstep(0.1, 0.25, f));
//