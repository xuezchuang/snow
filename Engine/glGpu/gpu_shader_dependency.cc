
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdarg.h>



#include "utildefines.h"
#include "irrArray.h"
#include "irrString.h"
#include "irrMap.h"

#include "gpu_shader_create_info.hh"

//#include "BLI_ghash.h"
//#include "BLI_map.hh"
//#include "BLI_string_ref.hh"
//
//#include "gpu_material_library.h"
//#include "gpu_shader_create_info.hh"
#include "gpu_shader_dependency_private.h"
//
//#include "GPU_context.h"
//
extern "C" {
#define SHADER_SOURCE(datatoc, filename, filepath) extern char datatoc[];
	//#include "glsl_compositor_source_list.h"
	//#include "glsl_draw_source_list.h"
#include "glsl_gpu_source_list.h"
#ifdef WITH_OCIO
#  include "glsl_ocio_source_list.h"
#endif
#undef SHADER_SOURCE
}

namespace irr::gpu
{
typedef enum
{
	FUNCTION_QUAL_IN,
	FUNCTION_QUAL_OUT,
	FUNCTION_QUAL_INOUT,
} GPUFunctionQual;

typedef enum eGPUType
{
	/* Keep in sync with GPU_DATATYPE_STR */
	/* The value indicates the number of elements in each type */
	GPU_NONE = 0,
	GPU_FLOAT = 1,
	GPU_VEC2 = 2,
	GPU_VEC3 = 3,
	GPU_VEC4 = 4,
	GPU_MAT3 = 9,
	GPU_MAT4 = 16,
	GPU_MAX_CONSTANT_DATA = GPU_MAT4,

	/* Values not in GPU_DATATYPE_STR */
	GPU_TEX1D_ARRAY = 1001,
	GPU_TEX2D = 1002,
	GPU_TEX2D_ARRAY = 1003,
	GPU_TEX3D = 1004,

	/* GLSL Struct types */
	GPU_CLOSURE = 1007,

	/* Opengl Attributes */
	GPU_ATTR = 3001,
} eGPUType;

#define MAX_FUNCTION_NAME 64
#define MAX_PARAMETER 36

typedef struct GPUFunction
{
	char name[MAX_FUNCTION_NAME];
	eGPUType paramtype[MAX_PARAMETER];
	GPUFunctionQual paramqual[MAX_PARAMETER];
	int totparam;
	/* TODO(@fclem): Clean that void pointer. */
	void* source; /* GPUSource */
} GPUFunction;

typedef std::string StringRefNull;
typedef std::string StringRef;

using GPUSourceDictionnary = core::map<StringRef, struct GPUSource*>;
using GPUFunctionDictionnary = core::map<StringRef, GPUFunction*>;

#define CHECK(test_value, str, ofs, msg) \
	if ((test_value) == -1) { \
		continue; \
	}



struct GPUSource
{
	StringRefNull fullpath;
	StringRefNull filename;
	StringRefNull source;
	core::array<GPUSource*> dependencies;
	bool dependencies_init = false;
	shader::BuiltinBits builtins = shader::BuiltinBits::NONE;
	std::string processed_source;
	GPUSource(const char* path,
			  const char* file,
			  const char* datatoc,
			  GPUFunctionDictionnary* g_functions)
		: fullpath(path), filename(file), source(datatoc)
	{
		/* Scan for builtins. */
		/* FIXME: This can trigger false positive caused by disabled #if blocks. */
		/* TODO(fclem): Could be made faster by scanning once. */
		if(source.find("gl_FragCoord", 0) != -1)
		{
			builtins |= shader::BuiltinBits::FRAG_COORD;
		}
		if(source.find("gl_FrontFacing", 0) != -1)
		{
			builtins |= shader::BuiltinBits::FRONT_FACING;
		}
		if(source.find("gl_GlobalInvocationID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::GLOBAL_INVOCATION_ID;
		}
		if(source.find("gl_InstanceID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::INSTANCE_ID;
		}
		if(source.find("gl_LocalInvocationID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::LOCAL_INVOCATION_ID;
		}
		if(source.find("gl_LocalInvocationIndex", 0) != -1)
		{
			builtins |= shader::BuiltinBits::LOCAL_INVOCATION_INDEX;
		}
		if(source.find("gl_NumWorkGroup", 0) != -1)
		{
			builtins |= shader::BuiltinBits::NUM_WORK_GROUP;
		}
		if(source.find("gl_PointCoord", 0) != -1)
		{
			builtins |= shader::BuiltinBits::POINT_COORD;
		}
		if(source.find("gl_PointSize", 0) != -1)
		{
			builtins |= shader::BuiltinBits::POINT_SIZE;
		}
		if(source.find("gl_PrimitiveID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::PRIMITIVE_ID;
		}
		if(source.find("gl_VertexID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::VERTEX_ID;
		}
		if(source.find("gl_WorkGroupID", 0) != -1)
		{
			builtins |= shader::BuiltinBits::WORK_GROUP_ID;
		}
		if(source.find("gl_WorkGroupSize", 0) != -1)
		{
			builtins |= shader::BuiltinBits::WORK_GROUP_SIZE;
		}
		/* TODO(fclem): We could do that at compile time. */
		/* Limit to shared header files to avoid the temptation to use C++ syntax in .glsl files. */
		if(endswith(".h") || endswith(".hh"))
		{
			enum_preprocess();
			quote_preprocess();
		}
		else
		{
			if(source.find("'") != -1)
			{
				//char_literals_preprocess();
			}
			if(source.find("drw_print") != -1)
			{
				//string_preprocess();
			}
			if((source.find("drw_debug_") != -1) &&
			   /* Avoid this file as it is a false positive match (matches "drw_debug_print_buf"). */
			   filename != "draw_debug_print_display_vert.glsl" &&
			   /* Avoid these two files where it makes no sense to add the dependency. */
			   !ELEM(filename, "common_debug_draw_lib.glsl", "draw_debug_draw_display_vert.glsl"))
			{
				builtins |= shader::BuiltinBits::USE_DEBUG_DRAW;
			}
			//check_no_quotes();
		}

		//if(is_from_material_library())
		//{
		//	material_functions_parse(g_functions);
		//}
	};

	static bool is_in_comment(const std::string& input, uint64_t offset)
	{
		return (input.rfind("/*", offset) > input.rfind("*/", offset)) ||
			(input.rfind("//", offset) > input.rfind("\n", offset));
	}

	template<bool check_whole_word = true, bool reversed = false, typename T>
	static int64_t find_str(const StringRef& input, const T keyword, int64_t offset = 0)
	{
		while(true)
		{
			if constexpr(reversed)
			{
				offset = input.rfind(keyword, offset);
			}
			else
			{
				offset = input.find(keyword, offset);
			}
			if(offset > 0)
			{
				if constexpr(check_whole_word)
				{
					/* Fix false positive if something has "enum" as suffix. */
					char previous_char = input[offset - 1];
					if(!ELEM(previous_char, '\n', '\t', ' ', ':', '(', ','))
					{
						offset += (reversed) ? -1 : 1;
						continue;
					}
				}
				/* Fix case where the keyword is in a comment. */
				if(is_in_comment(input.c_str(), offset))
				{
					offset += (reversed) ? -1 : 1;
					continue;
				}
			}
			return offset;
		}
	}

#define find_keyword find_str<true, false>
#define rfind_keyword find_str<true, true>
#define find_token find_str<false, false>
#define rfind_token find_str<false, true>

	size_t BLI_vsnprintf(char* __restrict dst,
						 size_t dst_maxncpy,
						 const char* __restrict format,
						 va_list arg)
	{
		size_t n;
		n = (size_t)vsnprintf(dst, dst_maxncpy, format, arg);
		if(n != -1 && n < dst_maxncpy)
		{
			dst[n] = '\0';
		}
		else
		{
			dst[dst_maxncpy - 1] = '\0';
		}
		return n;
	}

	size_t BLI_snprintf(char* __restrict dst, size_t dst_maxncpy, const char* __restrict format, ...)
	{

		size_t n;
		va_list arg;

		va_start(arg, format);
		n = BLI_vsnprintf(dst, dst_maxncpy, format, arg);
		va_end(arg);

		return n;
	}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))
#define SNPRINTF(dst, format, ...) BLI_snprintf(dst, ARRAY_SIZE(dst), format, __VA_ARGS__)

	/*constexpr*/ bool endswith(const StringRef& suffix) const
	{
		if(filename.size() < suffix.size())
		{
			return false;
		}
		const size_t offset = filename.size() - suffix.size();
		for(size_t i = 0; i < suffix.size(); i++)
		{
			if(filename[offset + i] != suffix[i])
			{
				return false;
			}
		}
		return true;
	}
	/*constexpr*/ bool startswith(const StringRef& prefix) const
	{
		if(filename.size() < prefix.size())
		{
			return false;
		}
		for(size_t i = 0; i < prefix.size(); i++)
		{
			if(filename[i] != prefix[i])
			{
				return false;
			}
		}
		return true;
	}
	void char_literals_preprocess()
	{
		const StringRefNull input = source;
		std::stringstream output;
		int64_t cursor = -1;
		int64_t last_pos = 0;

		while(true)
		{
			cursor = find_token(input, '\'', cursor + 1);
			if(cursor == -1)
			{
				break;
			}
			/* Output anything between 2 print statement. */
			output << input.substr(last_pos, cursor - last_pos);

			/* Extract string. */
			int64_t char_start = cursor + 1;
			int64_t char_end = find_token(input, '\'', char_start);
			CHECK(char_end, input, cursor, "Malformed char literal. Missing ending `'`.");

			StringRef input_char = input.substr(char_start, char_end - char_start);
			if(input_char.size() == 0)
			{
				CHECK(-1, input, cursor, "Malformed char literal. Empty character constant");
			}

			uint8_t char_value = input_char[0];

			if(input_char[0] == '\\')
			{
				if(input_char[1] == 'n')
				{
					char_value = '\n';
				}
				else
				{
					CHECK(-1, input, cursor, "Unsupported escaped character");
				}
			}
			else
			{
				if(input_char.size() > 1)
				{
					CHECK(-1, input, cursor, "Malformed char literal. Multi-character character constant");
				}
			}

			char hex[8];
			SNPRINTF(hex, "0x%.2Xu", char_value);
			output << hex;

			cursor = last_pos = char_end + 1;
		}
		/* If nothing has been changed, do not allocate processed_source. */
		if(last_pos == 0)
		{
			return;
		}

		if(last_pos != 0)
		{
			output << input.substr(last_pos);
		}
		processed_source = output.str();
		source = processed_source.c_str();
	}

	bool is_from_material_library() const
	{
		return (startswith("gpu_shader_material_") ||
				startswith("gpu_shader_common_") ||
				startswith("gpu_shader_compositor_")) &&
			endswith(".glsl");
	}

	void quote_preprocess()
	{
		if(source.find_first_of('"') == -1)
		{
			return;
		}

		processed_source = source.c_str();
		std::replace(processed_source.begin(), processed_source.end(), '"', ' ');

		source = processed_source.c_str();
	}
	void enum_preprocess()
	{
		const StringRefNull input = source;
		std::string output;
		int64_t cursor = -1;
		int64_t last_pos = 0;
		const bool is_cpp = endswith(".hh");

		/* Metal Shading language is based on C++ and supports C++-style enumerations.
		 * For these cases, we do not need to perform auto-replacement. */
		if(is_cpp /*&& GPU_backend_get_type() == GPU_BACKEND_METAL*/)
		{
			return;
		}

		while(true)
		{
			cursor = find_keyword(input, "enum ", cursor + 1);
			if(cursor == -1)
			{
				break;
			}
			/* Skip matches like `typedef enum myEnum myType;` */
			if(cursor >= 8 && input.substr(cursor - 8, 8) == "typedef ")
			{
				continue;
			}
			/* Output anything between 2 enums blocks. */
			output += input.substr(last_pos, cursor - last_pos);

			/* Extract enum type name. */
			int64_t name_start = input.find(" ", cursor);

			int64_t values_start = find_token(input, '{', cursor);
			CHECK(values_start, input, cursor, "Malformed enum class. Expected \'{\' after typename.");

			StringRef enum_name = input.substr(name_start, values_start - name_start);
			if(is_cpp)
			{
				int64_t name_end = find_token(enum_name, ":");
				CHECK(name_end, input, name_start, "Expected \':\' after C++ enum name.");

				int64_t underlying_type = find_keyword(enum_name, "uint32_t", name_end);
				CHECK(underlying_type, input, name_start, "C++ enums needs uint32_t underlying type.");

				enum_name = input.substr(name_start, name_end);
			}

			output += "#define " + enum_name + " uint\n";

			/* Extract enum values. */
			int64_t values_end = find_token(input, '}', values_start);
			CHECK(values_end, input, cursor, "Malformed enum class. Expected \'}\' after values.");

			/* Skip opening brackets. */
			values_start += 1;

			StringRef enum_values = input.substr(values_start, values_end - values_start);

			/* Really poor check. Could be done better. */
			int64_t token = find_token(enum_values, '{');
			int64_t not_found = (token == -1) ? 0 : -1;
			CHECK(not_found, input, values_start + token, "Unexpected \'{\' token inside enum values.");

			/* Do not capture the comma after the last value (if present). */
			int64_t last_equal = rfind_token(enum_values, '=', values_end);
			int64_t last_comma = rfind_token(enum_values, ',', values_end);
			if(last_comma > last_equal)
			{
				enum_values = input.substr(values_start, last_comma);
			}

			output += "const uint " + enum_values;

			int64_t semicolon_found = (input[values_end + 1] == ';') ? 0 : -1;
			CHECK(semicolon_found, input, values_end + 1, "Expected \';\' after enum type declaration.");

			/* Skip the curly bracket but not the semicolon. */
			cursor = last_pos = values_end + 1;
		}
		/* If nothing has been changed, do not allocate processed_source. */
		if(last_pos == 0)
		{
			return;
		}

		if(last_pos != 0)
		{
			output += input.substr(last_pos);
		}

		processed_source = output;
		source = processed_source.c_str();
	};
	/* Return 1 one error. */
	int init_dependencies(const GPUSourceDictionnary& dict, const GPUFunctionDictionnary& g_functions)
	{
		if(this->dependencies_init)
		{
			return 0;
		}
		this->dependencies_init = true;
		int64_t pos = -1;

		using namespace shader;
		/* Auto dependency injection for debug capabilities. */
		if((builtins & BuiltinBits::USE_DEBUG_DRAW) == BuiltinBits::USE_DEBUG_DRAW)
		{
			dependencies.push_back(dict.find("common_debug_draw_lib.glsl")->getValue());
		}
		if((builtins & BuiltinBits::USE_DEBUG_PRINT) == BuiltinBits::USE_DEBUG_PRINT)
		{
			dependencies.push_back(dict.find("common_debug_print_lib.glsl")->getValue());
		}

		while(true)
		{
			GPUSource* dependency_source = nullptr;
			{
				pos = source.find("pragma BLENDER_REQUIRE(", pos + 1);
				if(pos == -1)
				{
					return 0;
				}
				int64_t start = source.find('(', pos) + 1;
				int64_t end = source.find(')', pos);
				if(end == -1)
				{
					//print_error(source,start,"Malformed BLENDER_REQUIRE: Missing \")\" token");
					return 1;
				}
				StringRef dependency_name = source.substr(start, end - start);
				//dependency_source = dict.lookup_default(dependency_name,nullptr);
				auto node = dict.find(dependency_name);
				if(node == nullptr)
				{
					//print_error(source,start,"Dependency not found");
					return 1;
				}
				else
				{
					dependency_source = node->getValue();
				}
			}

			/* Recursive. */
			int result = dependency_source->init_dependencies(dict, g_functions);
			if(result != 0)
			{
				return 1;
			}

			for(u32 i = 0; i < dependency_source->dependencies.size(); i++)
			{
				dependencies.push_back(dependency_source->dependencies[i]);
				//dependencies.push_back(dep->getValue());
			}
			dependencies.push_back(dependency_source);
		}
		/* Precedes an eternal loop (quiet CLANG's `unreachable-code` warning). */
		BLI_assert_unreachable();
		return 0;
	}
	/* Returns the final string with all includes done. */
	void build(std::string& result) const
	{
		for(u32 i = 0; i < dependencies.size(); ++i)
		{
			result.append(dependencies[i]->source.c_str());
		}
		result.append(source.c_str());
	}
};

}  // namespace blender::gpu
using namespace irr::gpu;



static GPUSourceDictionnary* g_sources = nullptr;
static GPUFunctionDictionnary* g_functions = nullptr;

void gpu_shader_dependency_init()
{
	g_sources = new GPUSourceDictionnary();
	g_functions = new GPUFunctionDictionnary();
#define SHADER_SOURCE(datatoc, filename, filepath)\
	g_sources->set(filename, new GPUSource(filepath, filename, datatoc, g_functions));
	//#include "glsl_compositor_source_list.h"
	//#include "glsl_draw_source_list.h"
#include "glsl_gpu_source_list.h"
#undef SHADER_SOURCE

	int errors = 0;

	GPUSourceDictionnary::Iterator Iterator;
	for(Iterator = g_sources->getIterator(); !Iterator.atEnd(); )
	{
		auto value = Iterator.getNode()->getValue();
		errors += value->init_dependencies(*g_sources, *g_functions);
		Iterator++;
	}

}



namespace irr::gpu::shader
{

//BuiltinBits gpu_shader_dependency_get_builtins(const StringRefNull shader_source_name)
//{
//	if(shader_source_name.is_empty())
//	{
//		return shader::BuiltinBits::NONE;
//	}
//	if(g_sources->contains(shader_source_name) == false)
//	{
//		std::cerr << "Error: Could not find \"" << shader_source_name
//			<< "\" in the list of registered source.\n";
//		BLI_assert(0);
//		return shader::BuiltinBits::NONE;
//	}
//	GPUSource* source = g_sources->lookup(shader_source_name);
//	return source->builtins_get();
//}
//
std::string gpu_shader_dependency_get_resolved_source(const StringRefNull shader_source_name)
{
	GPUSourceDictionnary::Node* node = g_sources->find(shader_source_name);
	if(node == nullptr)
	{
		std::cerr << "Error source not found : " << shader_source_name << std::endl;
	}
	std::string result;
	GPUSource* src = node->getValue();
	src->build(result);
	return result;
}

//StringRefNull gpu_shader_dependency_get_source(const StringRefNull shader_source_name)
//{
//	GPUSource* src = g_sources->lookup_default(shader_source_name,nullptr);
//	if(src == nullptr)
//	{
//		std::cerr << "Error source not found : " << shader_source_name << std::endl;
//	}
//	return src->source;
//}
//
//StringRefNull gpu_shader_dependency_get_filename_from_source_string(
//	const StringRefNull source_string)
//{
//	for(auto& source : g_sources->values())
//	{
//		if(source->source.c_str() == source_string.c_str())
//		{
//			return source->filename;
//		}
//	}
//	return "";
//}

}  // namespace blender::gpu::shader
