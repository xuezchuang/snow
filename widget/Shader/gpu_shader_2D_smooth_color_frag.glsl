#version 410
noperspective in vec4 finalColor;
out vec4 fragColor;

void main()
{
	fragColor = finalColor;
	//fragColor = vec4(1,0,0,1);//finalColor;
  
  //fragColor = blender_srgb_to_framebuffer_space(fragColor);

    //vec3 c = max(fragColor.rgb, vec3(0.0));
    //vec3 c1 = c * (1.0 / 12.92);
    //vec3 c2 = pow((c + 0.055) * (1.0 / 1.055), vec3(2.4));
    //fragColor.rgb = mix(c1, c2, step(vec3(0.04045), c));
}
