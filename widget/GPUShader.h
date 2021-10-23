#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef __cplusplus
extern "C" {
#endif
	typedef enum eGPUBuiltinShader {
		/* specialized drawing */
		GPU_SHADER_2D_SMOOTH_COLOR,
		GPU_SHADER_2D_UNIFORM_COLOR,
		GPU_SHADER_3D_TEST,
		render_prog,
	}eGPUBuiltinShader;
#define GPU_SHADER_BUILTIN_LEN (render_prog + 1)
#ifdef __cplusplus
}
#endif

struct GPUShader {
	GLuint program;
	GLuint vertex;
	GLuint geometry;
	GLuint fragment;
	//GPUShaderInterface* interface;
	int feedback_transform_type;
#ifndef NDEBUG
	char name[64];
#endif
};
class IGPUShader
{
public:
	static IGPUShader* Instance();
	static void Clear();
	static bool isNull();
public:
	GPUShader* GetShader(eGPUBuiltinShader shader);
	void bindShader(eGPUBuiltinShader shader);
protected:
	IGPUShader();
	virtual ~IGPUShader();
private:
	static IGPUShader* _instance;
	GPUShader* m_GPUShader[GPU_SHADER_BUILTIN_LEN];
};



