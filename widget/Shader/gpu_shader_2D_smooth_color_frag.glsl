#version 330
in vec4 finalColor;
out vec4 fragColor;

void main()
{
	fragColor = finalColor;
 // fragColor = vec4(1,0,0,1);//finalColor;
  
  //fragColor = blender_srgb_to_framebuffer_space(fragColor);
}
