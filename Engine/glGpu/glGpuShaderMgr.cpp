#include "glGpuShaderMgr.h"
//#include "gpu_shader_create_info_list.hh"
#include "GPU_shader.h"
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

const GPUShaderCreateInfo* CGpuShaderMgr::GetGpuShader(core::stringc str)
{
	auto pNode = mapCreateInfoDictionnary.find(str);
	if(pNode)
	{
		const shader::ShaderCreateInfo* _info = pNode->getValue();
		return reinterpret_cast<const GPUShaderCreateInfo*>(_info);
	}
	return nullptr;
}


} // end namespace
} // end namespace

