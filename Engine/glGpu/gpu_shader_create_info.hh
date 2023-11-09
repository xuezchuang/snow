#pragma once

#include "irrMap.h"

//#include "BLI_string_ref.hh"
//#include "BLI_vector.hh"
//#include "GPU_material.h"
//#include "GPU_texture.h"

#include <iostream>
#include <vector>

#include "GPU_texture.h"

#ifdef __cplusplus
/* Useful to port C code using enums to C++ where enums are strongly typed.
 * To use after the enum declaration. */
 /* If any enumerator `C` is set to say `A|B`, then `C` would be the max enum value. */
#define ENUM_OPERATORS(_enum_type, _max_enum_value) \
extern "C++" { \
    inline constexpr _enum_type operator|(_enum_type a, _enum_type b) \
    { \
      return (_enum_type)(uint64_t(a) | uint64_t(b)); \
    } \
    inline constexpr _enum_type operator&(_enum_type a, _enum_type b) \
    { \
      return (_enum_type)(uint64_t(a) & uint64_t(b)); \
    } \
    inline constexpr _enum_type operator~(_enum_type a) \
    { \
      return (_enum_type)(~uint64_t(a) & (2 * uint64_t(_max_enum_value) - 1)); \
    } \
    inline _enum_type &operator|=(_enum_type &a, _enum_type b) \
    { \
      return a = (_enum_type)(uint64_t(a) | uint64_t(b)); \
    } \
    inline _enum_type &operator&=(_enum_type &a, _enum_type b) \
    { \
      return a = (_enum_type)(uint64_t(a) & uint64_t(b)); \
    } \
    inline _enum_type &operator^=(_enum_type &a, _enum_type b) \
    { \
      return a = (_enum_type)(uint64_t(a) ^ uint64_t(b)); \
    } \
} /* extern "C++" */
#endif


namespace irr::gpu::shader
{
/* Helps intellisense / auto-completion. */
#ifndef GPU_SHADER_CREATE_INFO
#define GPU_SHADER_INTERFACE_INFO(_interface, _inst_name) \
    StageInterfaceInfo _interface(#_interface, _inst_name); \
    _interface
#define GPU_SHADER_CREATE_INFO(_info) \
    ShaderCreateInfo _info(#_info); \
    _info
#endif

typedef std::string StringRefNull;
typedef std::string StringRef;
template <typename T>
using Vector = std::vector<T>;
//typedef std::vector Vector;

enum class BuiltinBits
{
	NONE = 0,
	/**
	 * Allow getting barycentric coordinates inside the fragment shader.
	 * \note Emulated on OpenGL.
	 */
	BARYCENTRIC_COORD = (1 << 0),
	FRAG_COORD = (1 << 2),
	FRONT_FACING = (1 << 4),
	GLOBAL_INVOCATION_ID = (1 << 5),
	INSTANCE_ID = (1 << 6),
	/**
	 * Allow setting the target layer when the output is a layered frame-buffer.
	 * \note Emulated through geometry shader on older hardware.
	 */
	LAYER = (1 << 7),
	LOCAL_INVOCATION_ID = (1 << 8),
	LOCAL_INVOCATION_INDEX = (1 << 9),
	NUM_WORK_GROUP = (1 << 10),
	POINT_COORD = (1 << 11),
	POINT_SIZE = (1 << 12),
	PRIMITIVE_ID = (1 << 13),
	VERTEX_ID = (1 << 14),
	WORK_GROUP_ID = (1 << 15),
	WORK_GROUP_SIZE = (1 << 16),

	/* Not a builtin but a flag we use to tag shaders that use the debug features. */
	USE_DEBUG_DRAW = (1 << 29),
	USE_DEBUG_PRINT = (1 << 30),
};
ENUM_OPERATORS(BuiltinBits, BuiltinBits::USE_DEBUG_PRINT);

/**
* Follow convention described in:
* https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_conservative_depth.txt
*/
enum class DepthWrite
{
	/* UNCHANGED specified as default to indicate gl_FragDepth is not used. */
	UNCHANGED = 0,
	ANY,
	GREATER,
	LESS,
};

/* Samplers & images. */
enum class ImageType
{
	/** Color samplers/image. */
	FLOAT_BUFFER = 0,
	FLOAT_1D,
	FLOAT_1D_ARRAY,
	FLOAT_2D,
	FLOAT_2D_ARRAY,
	FLOAT_3D,
	FLOAT_CUBE,
	FLOAT_CUBE_ARRAY,
	INT_BUFFER,
	INT_1D,
	INT_1D_ARRAY,
	INT_2D,
	INT_2D_ARRAY,
	INT_3D,
	INT_CUBE,
	INT_CUBE_ARRAY,
	UINT_BUFFER,
	UINT_1D,
	UINT_1D_ARRAY,
	UINT_2D,
	UINT_2D_ARRAY,
	UINT_3D,
	UINT_CUBE,
	UINT_CUBE_ARRAY,
	/** Depth samplers (not supported as image). */
	SHADOW_2D,
	SHADOW_2D_ARRAY,
	SHADOW_CUBE,
	SHADOW_CUBE_ARRAY,
	DEPTH_2D,
	DEPTH_2D_ARRAY,
	DEPTH_CUBE,
	DEPTH_CUBE_ARRAY,
};

enum class Type
{
	/* Types supported natively across all GPU back-ends. */
	FLOAT = 0,
	VEC2,
	VEC3,
	VEC4,
	MAT3,
	MAT4,
	UINT,
	UVEC2,
	UVEC3,
	UVEC4,
	INT,
	IVEC2,
	IVEC3,
	IVEC4,
	BOOL,
	/* Additionally supported types to enable data optimization and native
	* support in some GPU back-ends.
	* NOTE: These types must be representable in all APIs. E.g. `VEC3_101010I2` is aliased as vec3
	* in the GL back-end, as implicit type conversions from packed normal attribute data to vec3 is
	* supported. UCHAR/CHAR types are natively supported in Metal and can be used to avoid
	* additional data conversions for `GPU_COMP_U8` vertex attributes. */
	VEC3_101010I2,
	UCHAR,
	UCHAR2,
	UCHAR3,
	UCHAR4,
	CHAR,
	CHAR2,
	CHAR3,
	CHAR4
};

/* Storage qualifiers. */
enum class Qualifier
{
	/** Restrict flag is set by default. Unless specified otherwise. */
	NO_RESTRICT = (1 << 0),
	READ = (1 << 1),
	WRITE = (1 << 2),
	/** Shorthand version of combined flags. */
	READ_WRITE = READ | WRITE,
	QUALIFIER_MAX = (WRITE << 1) - 1,
};
ENUM_OPERATORS(Qualifier, Qualifier::QUALIFIER_MAX);

enum class Frequency
{
	BATCH = 0,
	PASS,
};

/* Dual Source Blending Index. */
enum class DualBlend
{
	NONE = 0,
	SRC_0,
	SRC_1,
};

/* Interpolation qualifiers. */
enum class Interpolation
{
	SMOOTH = 0,
	FLAT,
	NO_PERSPECTIVE,
};

/** Input layout for geometry shader. */
enum class PrimitiveIn
{
	POINTS = 0,
	LINES,
	LINES_ADJACENCY,
	TRIANGLES,
	TRIANGLES_ADJACENCY,
};

/** Output layout for geometry shader. */
enum class PrimitiveOut
{
	POINTS = 0,
	LINE_STRIP,
	TRIANGLE_STRIP,
	LINES,
	TRIANGLES,
};

struct StageInterfaceInfo
{
	struct InOut
	{
		Interpolation interp;
		Type type;
		StringRefNull name;
	};

	StringRefNull name;
	/**
	 * Name of the instance of the block (used to access).
	 * Can be empty string (i.e: "") only if not using geometry shader.
	 */
	StringRefNull instance_name;
	/** List of all members of the interface. */
	Vector<InOut> inouts;

	StageInterfaceInfo(const char* name_, const char* instance_name_) : name(name_), instance_name(instance_name_) {};
	~StageInterfaceInfo() {};

	using Self = StageInterfaceInfo;

	Self& smooth(Type type, StringRefNull _name)
	{
		//inouts.append({Interpolation::SMOOTH, type, _name});
		inouts.push_back({Interpolation::SMOOTH, type, _name});
		return *(Self*)this;
	}

	Self& flat(Type type, StringRefNull _name)
	{
		//inouts.append({Interpolation::FLAT, type, _name});
		inouts.push_back({Interpolation::FLAT, type, _name});
		return *(Self*)this;
	}

	Self& no_perspective(Type type, StringRefNull _name)
	{
		//inouts.append({Interpolation::NO_PERSPECTIVE, type, _name});
		inouts.push_back({Interpolation::NO_PERSPECTIVE, type, _name});
		return *(Self*)this;
	}
};


struct ShaderCreateInfo
{
	StringRefNull name_;
	/** True if the shader is static and can be pre-compiled at compile time. */
	bool do_static_compilation_ = false;
	/** If true, all additionally linked create info will be merged into this one. */
	bool finalized_ = false;
	/** If true, all resources will have an automatic location assigned. */
	bool auto_resource_location_ = false;
	/** If true, force depth and stencil tests to always happen before fragment shader invocation. */
	bool early_fragment_test_ = false;
	/** If true, force the use of the GL shader introspection for resource location. */
	bool legacy_resource_location_ = false;
	/** Allow optimization when fragment shader writes to `gl_FragDepth`. */
	DepthWrite depth_write_ = DepthWrite::UNCHANGED;
	/** GPU Backend compatibility flag. Temporary requirement until Metal enablement is fully
	 * complete. */
	bool metal_backend_only_ = false;
	/**
	 * Maximum length of all the resource names including each null terminator.
	 * Only for names used by #gpu::ShaderInterface.
	 */
	size_t interface_names_size_ = 0;
	/** Manually set builtins. */
	BuiltinBits builtins_ = BuiltinBits::NONE;
	/** Manually set generated code. */
	std::string vertex_source_generated = "";
	std::string fragment_source_generated = "";
	std::string compute_source_generated = "";
	std::string geometry_source_generated = "";
	std::string typedef_source_generated = "";
	/** Manually set generated dependencies. */
	//Vector<const char*,0> dependencies_generated;

#define TEST_EQUAL(a, b, _member) \
	if (!((a)._member == (b)._member)) { \
		return false; \
	}

#define TEST_VECTOR_EQUAL(a, b, _vector) \
	TEST_EQUAL(a, b, _vector.size()); \
	for (auto i : _vector.index_range()) { \
		TEST_EQUAL(a, b, _vector[i]); \
	}

	struct PushConst
	{
		Type type;
		StringRefNull name;
		int array_size;

		bool operator==(const PushConst& b) const
		{
			TEST_EQUAL(*this, b, type);
			TEST_EQUAL(*this, b, name);
			TEST_EQUAL(*this, b, array_size);
			return true;
		}
	};
	Vector<PushConst> push_constants_;
	/* Sources for resources type definitions. */
	Vector<StringRefNull> typedef_sources_;

	StringRefNull vertex_source_, geometry_source_, fragment_source_, compute_source_;

	struct VertIn
	{
		int index;
		Type type;
		StringRefNull name;

		bool operator==(const VertIn& b) const
		{
			TEST_EQUAL(*this, b, index);
			TEST_EQUAL(*this, b, type);
			TEST_EQUAL(*this, b, name);
			return true;
		}
	};

	Vector<VertIn> vertex_inputs_;

	struct GeometryStageLayout
	{
		PrimitiveIn primitive_in;
		int invocations;
		PrimitiveOut primitive_out;
		/** Set to -1 by default to check if used. */
		int max_vertices = -1;

		bool operator==(const GeometryStageLayout& b)
		{
			TEST_EQUAL(*this, b, primitive_in);
			TEST_EQUAL(*this, b, invocations);
			TEST_EQUAL(*this, b, primitive_out);
			TEST_EQUAL(*this, b, max_vertices);
			return true;
		}
	};
	GeometryStageLayout geometry_layout_;

	struct ComputeStageLayout
	{
		int local_size_x = -1;
		int local_size_y = -1;
		int local_size_z = -1;

		bool operator==(const ComputeStageLayout& b)
		{
			TEST_EQUAL(*this, b, local_size_x);
			TEST_EQUAL(*this, b, local_size_y);
			TEST_EQUAL(*this, b, local_size_z);
			return true;
		}
	};
	ComputeStageLayout compute_layout_;

	struct FragOut
	{
		int index;
		Type type;
		DualBlend blend;
		StringRefNull name;

		bool operator==(const FragOut& b) const
		{
			TEST_EQUAL(*this, b, index);
			TEST_EQUAL(*this, b, type);
			TEST_EQUAL(*this, b, blend);
			TEST_EQUAL(*this, b, name);
			return true;
		}
	};
	Vector<FragOut> fragment_outputs_;

	//Vector<std::array<StringRefNull,2i>> defines_;
	Vector<StringRefNull> defines_;
	/**
	 * Name of other infos to recursively merge with this one.
	 * No data slot must overlap otherwise we throw an error.
	 */
	Vector<StringRefNull> additional_infos_;

	struct Sampler
	{
		ImageType type;
		GPUSamplerState sampler;
		StringRefNull name;
	};

	struct Image
	{
		eGPUTextureFormat format;
		ImageType type;
		Qualifier qualifiers;
		StringRefNull name;
	};

	struct UniformBuf
	{
		StringRefNull type_name;
		StringRefNull name;
	};

	struct StorageBuf
	{
		Qualifier qualifiers;
		StringRefNull type_name;
		StringRefNull name;
	};

	struct Resource
	{
		enum BindType
		{
			UNIFORM_BUFFER = 0,
			STORAGE_BUFFER,
			SAMPLER,
			IMAGE,
		};

		BindType bind_type;
		int slot;
		union
		{
			Sampler sampler;
			Image image;
			UniformBuf uniformbuf;
			StorageBuf storagebuf;
		};

		Resource(BindType type, int _slot) : bind_type(type), slot(_slot) {};
		virtual ~Resource() {}

		// implicit
		//Resource(const Resource& src)=delete;
		Resource(const Resource& src)
		{
			memcpy(this, &src, sizeof(this));
		}

		bool operator==(const Resource& b) const
		{
			TEST_EQUAL(*this, b, bind_type);
			TEST_EQUAL(*this, b, slot);
			switch(bind_type)
			{
			case UNIFORM_BUFFER:
				TEST_EQUAL(*this, b, uniformbuf.type_name);
				TEST_EQUAL(*this, b, uniformbuf.name);
				break;
			case STORAGE_BUFFER:
				TEST_EQUAL(*this, b, storagebuf.qualifiers);
				TEST_EQUAL(*this, b, storagebuf.type_name);
				TEST_EQUAL(*this, b, storagebuf.name);
				break;
			case SAMPLER:
				TEST_EQUAL(*this, b, sampler.type);
				TEST_EQUAL(*this, b, sampler.sampler);
				TEST_EQUAL(*this, b, sampler.name);
				break;
			case IMAGE:
				TEST_EQUAL(*this, b, image.format);
				TEST_EQUAL(*this, b, image.type);
				TEST_EQUAL(*this, b, image.qualifiers);
				TEST_EQUAL(*this, b, image.name);
				break;
			}
			return true;
		}
	};
	/**
	* Resources are grouped by frequency of change.
	* Pass resources are meant to be valid for the whole pass.
	* Batch resources can be changed in a more granular manner (per object/material).
	* Mis-usage will only produce suboptimal performance.
	*/
	Vector<Resource> pass_resources_, batch_resources_;

	Vector<StageInterfaceInfo*> vertex_out_interfaces_;
	Vector<StageInterfaceInfo*> geometry_out_interfaces_;

public:
	ShaderCreateInfo(const char* name) /*: name_(name)*/ {};
	~ShaderCreateInfo() {};

	using Self = ShaderCreateInfo;

	/************************************************************************/
	/*		name Shaders in/outs (fixed function pipeline config)			*/
	/************************************************************************/

	Self& vertex_in(int slot, Type type, StringRefNull name)
	{
		//vertex_inputs_.append({slot, type, name});
		//interface_names_size_ += name.size() + 1;
		return *(Self*)this;
	}

	Self& vertex_out(StageInterfaceInfo& interface)
	{
		vertex_out_interfaces_.push_back(&interface);
		return *(Self*)this;
	}

	/**
	 * IMPORTANT: invocations count is only used if GL_ARB_gpu_shader5 is supported. On
	 * implementations that do not supports it, the max_vertices will be multiplied by invocations.
	 * Your shader needs to account for this fact. Use `#ifdef GPU_ARB_gpu_shader5` and make a code
	 * path that does not rely on #gl_InvocationID.
	 */
	 //Self& geometry_layout(PrimitiveIn prim_in,
	 //					  PrimitiveOut prim_out,
	 //					  int max_vertices,
	 //					  int invocations = -1)
	 //{
	 //	geometry_layout_.primitive_in = prim_in;
	 //	geometry_layout_.primitive_out = prim_out;
	 //	geometry_layout_.max_vertices = max_vertices;
	 //	geometry_layout_.invocations = invocations;
	 //	return *(Self*)this;
	 //}

	 //Self& local_group_size(int local_size_x = -1,int local_size_y = -1,int local_size_z = -1)
	 //{
	 //	compute_layout_.local_size_x = local_size_x;
	 //	compute_layout_.local_size_y = local_size_y;
	 //	compute_layout_.local_size_z = local_size_z;
	 //	return *(Self*)this;
	 //}

	 /**
	  * Force fragment tests before fragment shader invocation.
	  * IMPORTANT: This is incompatible with using the gl_FragDepth output.
	  */
	Self& early_fragment_test(bool enable)
	{
		early_fragment_test_ = enable;
		return *(Self*)this;
	}

	/**
	 * Only needed if geometry shader is enabled.
	 * IMPORTANT: Input and output instance name will have respectively "_in" and "_out" suffix
	 * appended in the geometry shader IF AND ONLY IF the vertex_out interface instance name matches
	 * the geometry_out interface instance name.
	 */
	Self& geometry_out(StageInterfaceInfo& interface)
	{
		geometry_out_interfaces_.push_back(&interface);
		return *(Self*)this;
	}

	Self& fragment_out(int slot, Type type, StringRefNull name, DualBlend blend = DualBlend::NONE)
	{
		fragment_outputs_.push_back({slot, type, blend, name});
		return *(Self*)this;
	}

	/************************************************************************/
	/*				name Resources bindings points							*/
	/************************************************************************/
	Self& uniform_buf(int slot,
					  StringRefNull type_name,
					  StringRefNull name,
					  Frequency freq = Frequency::PASS)
	{
		Resource res(Resource::BindType::UNIFORM_BUFFER, slot);
		res.uniformbuf.name = name;
		res.uniformbuf.type_name = type_name;
		((freq == Frequency::PASS) ? pass_resources_ : batch_resources_).push_back(res);
		interface_names_size_ += name.size() + 1;
		return *(Self*)this;
	}

	Self& storage_buf(int slot,
					  Qualifier qualifiers,
					  StringRefNull type_name,
					  StringRefNull name,
					  Frequency freq = Frequency::PASS)
	{
		Resource res(Resource::BindType::STORAGE_BUFFER, slot);
		res.storagebuf.qualifiers = qualifiers;
		res.storagebuf.type_name = type_name;
		res.storagebuf.name = name;
		((freq == Frequency::PASS) ? pass_resources_ : batch_resources_).push_back(res);
		interface_names_size_ += name.size() + 1;
		return *(Self*)this;
	}

	//Self& image(int slot,
	//			eGPUTextureFormat format,
	//			Qualifier qualifiers,
	//			ImageType type,
	//			StringRefNull name,
	//			Frequency freq = Frequency::PASS)
	//{
	//	Resource res(Resource::BindType::IMAGE,slot);
	//	res.image.format = format;
	//	res.image.qualifiers = qualifiers;
	//	res.image.type = type;
	//	res.image.name = name;
	//	((freq == Frequency::PASS) ? pass_resources_ : batch_resources_).append(res);
	//	interface_names_size_ += name.size() + 1;
	//	return *(Self*)this;
	//}

	//Self& sampler(int slot,
	//			  ImageType type,
	//			  StringRefNull name,
	//			  Frequency freq = Frequency::PASS,
	//			  GPUSamplerState sampler = GPUSamplerState::internal_sampler())
	//{
	//	Resource res(Resource::BindType::SAMPLER,slot);
	//	res.sampler.type = type;
	//	res.sampler.name = name;
	//	/* Produces ASAN errors for the moment. */
	//	// res.sampler.sampler = sampler;
	//	UNUSED_VARS(sampler);
	//	((freq == Frequency::PASS) ? pass_resources_ : batch_resources_).append(res);
	//	interface_names_size_ += name.size() + 1;
	//	return *(Self*)this;
	//}

	/************************************************************************/
	/*							name Shader Source							*/
	/************************************************************************/
	Self& vertex_source(StringRefNull filename)
	{
		vertex_source_ = filename;
		return *(Self*)this;
	}

	Self& geometry_source(StringRefNull filename)
	{
		geometry_source_ = filename;
		return *(Self*)this;
	}

	Self& fragment_source(StringRefNull filename)
	{
		fragment_source_ = filename;
		return *(Self*)this;
	}

	Self& compute_source(StringRefNull filename)
	{
		compute_source_ = filename;
		return *(Self*)this;
	}

	/************************************************************************/
	/*							name Push constants							*/
	/************************************************************************/
	Self& push_constant(Type type, StringRefNull name, int array_size = 0)
	{
		//BLI_assert_msg(name.find("[") == -1,
		//			   "Array syntax is forbidden for push constants."
		//			   "Use the array_size parameter instead.");
		push_constants_.push_back({type, name, array_size});
		interface_names_size_ += name.size() + 1;
		return *(Self*)this;
	}

	/************************************************************************/
	/*							name Defines								*/
	/************************************************************************/
	Self& define(StringRefNull name, StringRefNull value = "")
	{
		defines_.push_back(name);
		defines_.push_back(value);
		return *(Self*)this;
	}

	Self& do_static_compilation(bool value)
	{
		do_static_compilation_ = value;
		return *(Self*)this;
	}

	Self& builtins(BuiltinBits builtin)
	{
		builtins_ |= builtin;
		return *(Self*)this;
	}

	/* Defines how the fragment shader will write to gl_FragDepth. */
	Self& depth_write(DepthWrite value)
	{
		depth_write_ = value;
		return *(Self*)this;
	}

	Self& auto_resource_location(bool value)
	{
		auto_resource_location_ = value;
		return *(Self*)this;
	}

	Self& legacy_resource_location(bool value)
	{
		legacy_resource_location_ = value;
		return *(Self*)this;
	}

	Self& metal_backend_only(bool flag)
	{
		metal_backend_only_ = flag;
		return *(Self*)this;
	}

	/************************************************************************/
	/*				name Additional Create Info								*/
	/************************************************************************/
	Self& additional_info(StringRefNull info_name)
	{
		additional_infos_.push_back(info_name);
		return *(Self*)this;
	}

	template<typename... Args> Self& additional_info(StringRefNull info_name, Args... args)
	{
		additional_info(info_name);
		additional_info(args...);
		return *(Self*)this;
	}

	/************************************************************************/
	/*					Recursive evaluation.								*/
	/************************************************************************/

	void finalize();

	std::string check_error() const;

	/** Error detection that some backend compilers do not complain about. */
	void validate_merge(const ShaderCreateInfo& other_info);
	void validate_vertex_attributes(const ShaderCreateInfo* other_info = nullptr);
};

}
