#include "../SwitchShader.hpp"
#include <string>


class Shader_raytrace: public SwitchShader
{
	

	friend class ShaderManager;
	struct Uniform
	{
		GLuint sphereBinding;
	}uniforms;
	
	public:
		Shader_raytrace();
		
	
		const Uniform& getUniforms() const {return uniforms;}
	
};


