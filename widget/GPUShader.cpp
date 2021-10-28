#include "GPUShader.h"
#include "LoadShaders.h"
#include < assert.h >
IGPUShader* IGPUShader::_instance = nullptr;
IGPUShader* IGPUShader::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new IGPUShader;
	}
	return _instance;
}

void IGPUShader::Clear()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}
bool IGPUShader::isNull()
{
	return _instance == nullptr;
}

IGPUShader::IGPUShader()
{
	typedef struct {
		const char* vert;
		/** Optional. */
		const char* geom;
		const char* frag;
		/** Optional. */
		const char* defs;
	} GPUShaderStages;
	GPUShaderStages builtin_shader_stages[GPU_SHADER_BUILTIN_LEN];
	builtin_shader_stages[GPU_SHADER_2D_SMOOTH_COLOR].vert = "./Shader/gpu_shader_2D_smooth_color_vert.glsl";
	builtin_shader_stages[GPU_SHADER_2D_SMOOTH_COLOR].frag = "./Shader/gpu_shader_2D_smooth_color_frag.glsl";

	builtin_shader_stages[GPU_SHADER_2D_UNIFORM_COLOR].vert = "./Shader/gpu_shader_uniform_color_vert.glsl";
	builtin_shader_stages[GPU_SHADER_2D_UNIFORM_COLOR].frag = "./Shader/gpu_shader_uniform_color_frag.glsl";


	builtin_shader_stages[GPU_SHADER_3D_TEST].vert = "./Shader/primitive_restart.vs.glsl";
	builtin_shader_stages[GPU_SHADER_3D_TEST].frag = "./Shader/primitive_restart.fs.glsl";


	builtin_shader_stages[render_prog].vert = "./Shader/gpu_shader_2D_widget_base_vert.glsl";
	builtin_shader_stages[render_prog].frag = "./Shader/gpu_shader_2D_widget_base_frag.glsl";
	for (int i = 0; i < GPU_SHADER_BUILTIN_LEN; i++)
	{
		ShaderInfo temp[] =
		{
			{ GL_VERTEX_SHADER, builtin_shader_stages[i].vert },
			{ GL_FRAGMENT_SHADER, builtin_shader_stages[i].frag },
			{ GL_NONE, NULL }
		};
		m_GPUShader[i] = new GPUShader();
		m_GPUShader[i]->program = LoadShaders(temp);
	}
}

IGPUShader::~IGPUShader()
{
}


GPUShader* IGPUShader::GetShader(eGPUBuiltinShader shader)
{
	return m_GPUShader[shader];
}

void IGPUShader::bindShader(eGPUBuiltinShader shader)
{
	GLuint program = GetShader(shader)->program;
	glUseProgram(program);

	modelview_matrix_loc = glGetUniformLocation(program, "model_matrix");
	project_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	assert(modelview_matrix_loc != -1);
	assert(project_matrix_loc != -1);
}
