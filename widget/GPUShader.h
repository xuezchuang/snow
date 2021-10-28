#pragma once
#include "GPU_common.h"
typedef enum eGPUBuiltinShader {
	/* specialized drawing */
	GPU_SHADER_2D_SMOOTH_COLOR,
	GPU_SHADER_2D_UNIFORM_COLOR,
	GPU_SHADER_3D_TEST,
	render_prog,
}eGPUBuiltinShader;
#define GPU_SHADER_BUILTIN_LEN (render_prog + 1)

typedef struct GPUShader {
	GLuint program;
	GLuint vertex;
	GLuint geometry;
	GLuint fragment;
	//GPUShaderInterface* interface;
	int feedback_transform_type;
	char name[64];
}GPUShader;

class IGPUShader
{
public:
	static IGPUShader* Instance();
	static void Clear();
	static bool isNull();
public:
	GPUShader* GetShader(eGPUBuiltinShader shader);
	void bindShader(eGPUBuiltinShader shader);
	GLuint GetCurProgram();
	//uint GPU_shaderinterface_attr(char* name);
protected:
	IGPUShader();
	virtual ~IGPUShader();
private:
	static IGPUShader* _instance;
	GPUShader* m_GPUShader[GPU_SHADER_BUILTIN_LEN];
	GLint modelview_matrix_loc;
	GLint project_matrix_loc;
	GLuint m_Curprogram;
};



