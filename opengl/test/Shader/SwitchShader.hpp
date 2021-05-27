#ifndef SWITCHSHADER_H
#define SWITCHSHADER_H

#include <GL/glew.h>
#include <string>
#include <array>
#include <iostream>
#include <map>
#include <queue>
#include <fstream>
#include <string.h>
#include <cassert>

//! This shader class is derived to provide access to all game shader programs

void getShaderLog(std::ofstream&, std::string);
std::array<GLuint, 3> loadShaders(std::ofstream&, std::string&, std::string&);
void bindGlobalUniformBlock(GLuint);
void bindViewUniformBlock(GLuint);
void bindLightUniformBlock(GLuint);


#define SH_raytrace 0

class SwitchShader
{
	friend class ShaderManager;
	protected:
		GLuint shader_id;
		
		GLuint shader_vp;
		GLuint shader_fp;
		
		static std::string globalSettingsUniform;	//!< Uniform block containing uniforms common to all shaders
		
	
	
	public:
	
		int id(){return shader_id;}
		void bind() const{glUseProgram(shader_id);
			
		}
		
		virtual ~SwitchShader();
		
		
};

#define UB_MatrixBinding 1
#define UB_ViewBinding 2
#define UB_LightBinding 3

#define UB_USER 4


class ShaderManager
{
	static std::map<int, SwitchShader*> shaders;
	static int bound;
	
	static std::map<std::string, GLuint> blockBindings;
	static int bbCount;

	
	public:
		static void init();
		static void bind(int s){shaders[s]->bind();bound=s;}
		static int getBound(){return bound;}
		static const SwitchShader* getShader(int i){return shaders[i];}
		
		template <typename T>
		static const typename T::Uniform& getBoundUniform()
		{
			const T* s = dynamic_cast<const T*>(shaders[bound]);
			assert(s != NULL);
			//TODO if s == NULL exception
			return s->getUniforms();
		}
		
		static int getBlockBinding(std::string);

		
		static std::string getVersion();
		static std::string getGlobalSettingsUniform();
		static std::string getViewSettingsUniform();
		static std::string getLightSettingsUniform();

};


#endif
