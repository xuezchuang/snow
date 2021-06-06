#version 330
out vec4 fragColor;
uniform sampler2D sprite_texture;
void main()
{
   // fragColor = texture(sprite_texture, gl_PointCoord);
    fragColor = vec4(100.0/255.0f, 160/255, 8.0f, 1.0f);//0.0, 0.6, 0.0, 1.0);
}
