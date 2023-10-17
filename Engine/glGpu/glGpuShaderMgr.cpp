#include "glGpuShaderMgr.h"
//#include "gpu_shader_create_info_list.hh"

#undef GPU_SHADER_INTERFACE_INFO
#undef GPU_SHADER_CREATE_INFO


namespace irr
{
namespace gpu
{
//! constructor
CGpuShaderMgr::CGpuShaderMgr()
{
}

CGpuShaderMgr::~CGpuShaderMgr()
{
}

CGpuShaderMgr* CGpuShaderMgr::Instance()
{
	static CGpuShaderMgr Ins;
	return &Ins;
}

void CGpuShaderMgr::set(core::stringc str, shader::StageInterfaceInfo* data)
{
	mapInterfaceDictionnary.set(str,data);
}

void CGpuShaderMgr::set(core::stringc str, shader::ShaderCreateInfo* data)
{ 
	mapCreateInfoDictionnary.set(str,data);
}

void CGpuShaderMgr::InitShader()
{
	//gpu_shader_dependency_init();
}

} // end namespace
} // end namespace

