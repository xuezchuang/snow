
#pragma once

//#include "GPU_shader.h"

#ifdef __cplusplus
extern "C" {
#endif

	void gpu_shader_create_info_init(void);
	void gpu_shader_create_info_exit(void);

	//bool gpu_shader_create_info_compile_all(void);

	/** Runtime create infos are not registered in the dictionary and cannot be searched. */
	//const GPUShaderCreateInfo* gpu_shader_create_info_get(const char* info_name);

#ifdef __cplusplus
}
#endif
