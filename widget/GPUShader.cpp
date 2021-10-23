#include "GPUShader.h"

#include "LoadShaders.h"
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
	/*m_pCmdBar = nullptr;
	m_pCmdMgr = nullptr;
	m_pStatusBar = nullptr;
	m_pProjectMgr = nullptr;
	m_pProductMgr = nullptr;
	m_pMenuMgr = nullptr;
	m_pCmdResMgr = nullptr;
	m_pResMgr = nullptr;
	m_pDockBar = nullptr;
	m_pViewMgr = nullptr;
	m_pUnitCtrl = nullptr;
	m_pDBMgr = nullptr;
	m_pPlugin = nullptr;
	m_pMainPlugin = nullptr;
	m_pEventMgr = nullptr;
	m_pLabelMgr = nullptr;
	m_pFigMakerMgr = nullptr;
	m_pGripEditorMgr = nullptr;
	m_pFigurePoolMgr = nullptr;
	m_pPluginSlot = nullptr;
	m_pLanguage = nullptr;
	m_pToolbar = nullptr;
	m_pServiceMgr = nullptr;
	m_pOptionMgr = nullptr;
	m_pFrmUtilMgr = nullptr;
	m_pLinkMgr = nullptr;
	m_pLockMgr = nullptr;
	m_pUserPrivileges = new IUserPrivileges();*/
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
	//if (m_pCmdMgr)
	//	m_pCmdMgr->exitCmd();
	//if (m_pPlugin)
	//	m_pPlugin->clearAll();
	//if (m_pPluginSlot)
	//	m_pPluginSlot->unLoad();
	//if (m_pMainPlugin)
	//	m_pMainPlugin->clearAll();
	////
	//_SAFE_DELETE(m_pCmdBar);
	//_SAFE_DELETE(m_pCmdMgr);
	//_SAFE_DELETE(m_pStatusBar);
	//_SAFE_DELETE(m_pProjectMgr);
	//_SAFE_DELETE(m_pProductMgr);
	//_SAFE_DELETE(m_pMenuMgr);
	//_SAFE_DELETE(m_pCmdResMgr);
	//_SAFE_DELETE(m_pResMgr);
	//_SAFE_DELETE(m_pDockBar);
	//_SAFE_DELETE(m_pViewMgr);
	//_SAFE_DELETE(m_pUnitCtrl);
	//_SAFE_DELETE(m_pDBMgr);

	//_SAFE_DELETE(m_pFigMakerMgr);
	//_SAFE_DELETE(m_pGripEditorMgr);
	//_SAFE_DELETE(m_pLanguage);
	//// 	
	//_SAFE_DELETE(m_pPlugin);
	//_SAFE_DELETE(m_pPluginSlot);
	//_SAFE_DELETE(m_pMainPlugin);
	//_SAFE_DELETE(m_pToolbar);
	////
	//_SAFE_DELETE(m_pEventMgr);
	//_SAFE_DELETE(m_pServiceMgr);
	//_SAFE_DELETE(m_pFrmUtilMgr);
	//_SAFE_DELETE(m_pLinkMgr);
	//_SAFE_DELETE(m_pLockMgr);
	//_SAFE_DELETE(m_pUserPrivileges);
}


GPUShader* IGPUShader::GetShader(eGPUBuiltinShader shader)
{
	return m_GPUShader[shader];
}

void IGPUShader::bindShader(eGPUBuiltinShader shader)
{
	glUseProgram(GetShader(shader)->program);
}
