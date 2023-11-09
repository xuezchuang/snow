#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	void gpu_shader_dependency_init(void);

	void gpu_shader_dependency_exit(void);

	typedef std::string StringRefNull;
#ifdef __cplusplus
}
#endif
//
//#ifdef __cplusplus
//
//#include "BLI_string_ref.hh"
//#include "BLI_vector.hh"
//
//#include "gpu_shader_create_info.hh"
//



namespace irr::gpu::shader
{
//
//BuiltinBits gpu_shader_dependency_get_builtins(const StringRefNull source_name);
//

std::string gpu_shader_dependency_get_resolved_source(const StringRefNull source_name);
//StringRefNull gpu_shader_dependency_get_source(const StringRefNull source_name);
//
///**
// * \brief Find the name of the file from which the given string was generated.
// * \return filename or empty string.
// * \note source_string needs to be identical to the one given by gpu_shader_dependency_get_source()
// */
//StringRefNull gpu_shader_dependency_get_filename_from_source_string(
//	const StringRefNull source_string);
//
}  // namespace blender::gpu::shader
//
//#endif
