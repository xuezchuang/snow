#pragma once
#include "irrArray.h"
#include "irrString.h"
#include "irrMap.h"

//#include "ICameraSceneNode.h"
//#include "SViewFrustum.h"


namespace irr
{
namespace gpu
{

namespace shader
{
struct ShaderCreateInfo;
struct StageInterfaceInfo;
}


class CGpuShaderMgr // : public IGpuShaderMgr
{
public:
	CGpuShaderMgr();
	virtual ~CGpuShaderMgr();

	static CGpuShaderMgr* Instance();

	void set(core::stringc str, shader::ShaderCreateInfo* data);
	void set(core::stringc str, shader::StageInterfaceInfo* data);
protected:	
	//void gpu_shader_dependency_init();

	void InitShader();

	core::map<core::stringc, shader::ShaderCreateInfo*> mapCreateInfoDictionnary;
	core::map<core::stringc, shader::StageInterfaceInfo*> mapInterfaceDictionnary;
};

} // end namespace
} // end namespace


