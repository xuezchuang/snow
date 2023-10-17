#pragma once

#ifdef __cplusplus
#  include <string>
#endif

#include "utildefines.h"

/************************************************************************/
/*							name Sampler State							*/
/************************************************************************/

/**
* The `GPUSamplerFiltering` bit flag specifies the enabled filtering options of a texture
* sampler.
*/
typedef enum GPUSamplerFiltering
{
	/**
	* Default sampler filtering with all options off.
	* It means no linear filtering, no mipmapping, and no anisotropic filtering.
	*/
	GPU_SAMPLER_FILTERING_DEFAULT = 0,
	/**
	* Enables hardware linear filtering.
	* Also enables linear interpolation between MIPS if GPU_SAMPLER_FILTERING_MIPMAP is set.
	*/
	GPU_SAMPLER_FILTERING_LINEAR = (1 << 0),
	/**
	* Enables mipmap access through shader samplers.
	* Also enables linear interpolation between mips if GPU_SAMPLER_FILTER is set, otherwise the mip
	* interpolation will be set to nearest.
	*
	* The following parameters are always left to their default values and can't be changed:
	* - TEXTURE_MIN_LOD is -1000.
	* - TEXTURE_MAX_LOD is 1000.
	* - TEXTURE_LOD_BIAS is 0.0f.
	*/
	GPU_SAMPLER_FILTERING_MIPMAP = (1 << 1),
	/**
	* Enable Anisotropic filtering. This only has effect if `GPU_SAMPLER_FILTERING_MIPMAP` is set.
	* The filtered result is implementation dependent.
	*
	* The maximum amount of samples is always set to its maximum possible value and can't be
	* changed, except by the user through the user preferences, see the use of U.anisotropic_filter.
	*/
	GPU_SAMPLER_FILTERING_ANISOTROPIC = (1 << 2),
} GPUSamplerFiltering;

ENUM_OPERATORS(GPUSamplerFiltering,GPU_SAMPLER_FILTERING_ANISOTROPIC)

/** The number of every possible filtering configuration. */
static const int GPU_SAMPLER_FILTERING_TYPES_COUNT = (GPU_SAMPLER_FILTERING_LINEAR |
													  GPU_SAMPLER_FILTERING_MIPMAP |
													  GPU_SAMPLER_FILTERING_ANISOTROPIC) +1;

/**
* The `GPUSamplerExtendMode` specifies how the texture will be extrapolated for out-of-bound
* texture sampling.
*/
typedef enum GPUSamplerExtendMode
{
	/**
	* Extrapolate by extending the edge pixels of the texture, in other words, the texture
	* coordinates are clamped.
	*/
	GPU_SAMPLER_EXTEND_MODE_EXTEND = 0,
	/** Extrapolate by repeating the texture. */
	GPU_SAMPLER_EXTEND_MODE_REPEAT,
	/** Extrapolate by repeating the texture with mirroring in a ping-pong fashion. */
	GPU_SAMPLER_EXTEND_MODE_MIRRORED_REPEAT,
	/**
	* Extrapolate using the value of TEXTURE_BORDER_COLOR, which is always set to a transparent
	* black color (0, 0, 0, 0) and can't be changed.
	*/
	GPU_SAMPLER_EXTEND_MODE_CLAMP_TO_BORDER,
} GPUSamplerExtendMode;

#define GPU_SAMPLER_EXTEND_MODES_COUNT (GPU_SAMPLER_EXTEND_MODE_CLAMP_TO_BORDER + 1)

/**
* The `GPUSamplerCustomType` specifies pre-defined sampler configurations with parameters that
* are not controllable using the GPUSamplerFiltering and GPUSamplerExtendMode options. Hence, the
* use of a custom sampler type is mutually exclusive with the use of the aforementioned enums.
*
* The parameters that needs to be set for those custom samplers are not added as yet another
* option inside the GPUSamplerState structure because every possible configuration of sampler
* states are generated, setup, and cached at startup, so adding yet another axis of variation will
* multiply the number of configurations that needs to be cached, which is not worth it due to the
* limited use of the parameters needed to setup those custom samplers.
*/
typedef enum GPUSamplerCustomType
{
	/**
	* Enable compare mode for depth texture. The depth texture must then be bound to a shadow
	* sampler. This is equivalent to:
	*
	* - GPU_SAMPLER_FILTERING_LINEAR.
	* - GPU_SAMPLER_EXTEND_MODE_EXTEND.
	*
	* And sets:
	*
	* - TEXTURE_COMPARE_MODE -> COMPARE_REF_TO_TEXTURE.
	* - TEXTURE_COMPARE_FUNC -> LEQUAL.
	*/
	GPU_SAMPLER_CUSTOM_COMPARE = 0,
	/**
	* Special icon sampler with custom LOD bias and interpolation mode. This sets:
	*
	* - TEXTURE_MAG_FILTER -> LINEAR.
	* - TEXTURE_MIN_FILTER -> LINEAR_MIPMAP_NEAREST.
	* - TEXTURE_LOD_BIAS   -> -0.5.
	*/
	GPU_SAMPLER_CUSTOM_ICON,
} GPUSamplerCustomType;

#define GPU_SAMPLER_CUSTOM_TYPES_COUNT (GPU_SAMPLER_CUSTOM_ICON + 1)

/**
* The `GPUSamplerStateType` specifies how the GPUSamplerState structure should be interpreted
* when passed around due to it being an overloaded type, see the documentation of each of the
* types for more information.
*/
typedef enum GPUSamplerStateType
{
	/**
	* The filtering, extend_x, and extend_yz members of the GPUSamplerState structure will be used
	* in setting up the sampler state for the texture. The custom_type member will be ignored in
	* that case.
	*/
	GPU_SAMPLER_STATE_TYPE_PARAMETERS = 0,
	/**
	* The filtering, extend_x, and extend_yz members of the GPUSamplerState structure will be
	* ignored, and the predefined custom parameters outlined in the documentation of
	* GPUSamplerCustomType will be used in setting up the sampler state for the texture.
	*/
	GPU_SAMPLER_STATE_TYPE_CUSTOM,
	/**
	* The members of the GPUSamplerState structure will be ignored and the internal sampler state of
	* the texture will be used. In other words, this is a signal value and stores no useful or
	* actual data.
	*/
	GPU_SAMPLER_STATE_TYPE_INTERNAL,
} GPUSamplerStateType;

/**
* The `GPUSamplerState` specifies the sampler state to bind a texture with.
*
* When the state type is set to GPU_SAMPLER_STATE_TYPE_CUSTOM or GPU_SAMPLER_STATE_TYPE_INTERNAL,
* the rest of the members of the structure will be ignored. However, we can't turn this structure
* into a union, because various functions merely temporally change the state type and expect the
* rest of the members' values to be retained when the state type is changed back to
* GPU_SAMPLER_STATE_TYPE_PARAMETERS. For the instance, a function might do the following and
* expect the original sampler state of the texture to be retained after disabling comparison mode:
*
* GPU_texture_compare_mode(texture, true);
* // Use the texture ...
* GPU_texture_compare_mode(texture, false);
*
*/
typedef struct GPUSamplerState
{
	/** Specifies the enabled filtering options for the sampler. */
	GPUSamplerFiltering filtering : 8;
	/**
	* Specifies how the texture will be extrapolated for out-of-bound texture sampling along the x
	* axis.
	*/
	GPUSamplerExtendMode extend_x : 4;
	/**
	* Specifies how the texture will be extrapolated for out-of-bound texture sampling along both
	* the y and z axis. There is no individual control for the z axis because 3D textures have
	* limited use, and when used, their extend mode is typically the same for all axis.
	*/
	GPUSamplerExtendMode extend_yz : 4;
	/** Specifies the type of sampler if the state type is GPU_SAMPLER_STATE_TYPE_CUSTOM. */
	GPUSamplerCustomType custom_type : 8;
	/** Specifies how the GPUSamplerState structure should be interpreted when passed around. */
	GPUSamplerStateType type : 8;

#ifdef __cplusplus
	/**
	* Constructs a sampler state with default filtering and extended extend in both x and y axis.
	* See the documentation on GPU_SAMPLER_FILTERING_DEFAULT and GPU_SAMPLER_EXTEND_MODE_EXTEND for
	* more information.
	*
	* GPU_SAMPLER_STATE_TYPE_PARAMETERS is set in order to utilize the aforementioned parameters, so
	* GPU_SAMPLER_CUSTOM_COMPARE is arbitrary, ignored, and irrelevant.
	*/
	static constexpr GPUSamplerState default_sampler()
	{
		return {GPU_SAMPLER_FILTERING_DEFAULT,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_CUSTOM_COMPARE,
			GPU_SAMPLER_STATE_TYPE_PARAMETERS};
	}

	/**
	* Constructs a sampler state that can be used to signal that the internal sampler of the texture
	* should be used instead. See the documentation on GPU_SAMPLER_STATE_TYPE_INTERNAL for more
	* information.
	*
	* GPU_SAMPLER_STATE_TYPE_INTERNAL is set in order to signal the use of the internal sampler of
	* the texture, so the rest of the options before it are arbitrary, ignored, and irrelevant.
	*/
	static constexpr GPUSamplerState internal_sampler()
	{
		return {GPU_SAMPLER_FILTERING_DEFAULT,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_CUSTOM_COMPARE,
			GPU_SAMPLER_STATE_TYPE_INTERNAL};
	}

	/**
	* Constructs a special sampler state that can be used sampler icons. See the documentation on
	* GPU_SAMPLER_CUSTOM_ICON for more information.
	*
	* GPU_SAMPLER_STATE_TYPE_CUSTOM is set in order to specify a custom sampler type, so the rest of
	* the options before it are arbitrary, ignored, and irrelevant.
	*/
	static constexpr GPUSamplerState icon_sampler()
	{
		return {GPU_SAMPLER_FILTERING_DEFAULT,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_CUSTOM_ICON,
			GPU_SAMPLER_STATE_TYPE_CUSTOM};
	}

	/**
	* Constructs a special sampler state for depth comparison. See the documentation on
	* GPU_SAMPLER_CUSTOM_COMPARE for more information.
	*
	* GPU_SAMPLER_STATE_TYPE_CUSTOM is set in order to specify a custom sampler type, so the rest of
	* the options before it are ignored and irrelevant, but they are set to sensible defaults in
	* case comparison mode is turned off, in which case, the sampler state will become equivalent to
	* GPUSamplerState::default_sampler().
	*/
	static constexpr GPUSamplerState compare_sampler()
	{
		return {GPU_SAMPLER_FILTERING_DEFAULT,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_EXTEND_MODE_EXTEND,
			GPU_SAMPLER_CUSTOM_COMPARE,
			GPU_SAMPLER_STATE_TYPE_CUSTOM};
	}

	/**
	* Enables the given filtering flags.
	*/
	void enable_filtering_flag(GPUSamplerFiltering filtering_flags)
	{
		this->filtering = this->filtering | filtering_flags;
	}

	/**
	* Disables the given filtering flags.
	*/
	void disable_filtering_flag(GPUSamplerFiltering filtering_flags)
	{
		this->filtering = this->filtering & ~filtering_flags;
	}

	/**
	* Enables the given filtering flags if the given test is true, otherwise, disables the given
	* filtering flags.
	*/
	void set_filtering_flag_from_test(GPUSamplerFiltering filtering_flags,bool test)
	{
		if(test)
		{
			this->enable_filtering_flag(filtering_flags);
		}
		else
		{
			this->disable_filtering_flag(filtering_flags);
		}
	}

	std::string to_string() const
	{
		if(this->type == GPU_SAMPLER_STATE_TYPE_INTERNAL)
		{
			return "internal";
		}

		if(this->type == GPU_SAMPLER_STATE_TYPE_CUSTOM)
		{
			switch(this->custom_type)
			{
			case GPU_SAMPLER_CUSTOM_COMPARE:
				return "compare";
				break;
			case GPU_SAMPLER_CUSTOM_ICON:
				return "icon";
				break;
			default:
				BLI_assert_unreachable();
				return "";
			}
		}

		/* The sampler state is of type PARAMETERS, so serialize the parameters. */
		BLI_assert(this->type == GPU_SAMPLER_STATE_TYPE_PARAMETERS);
		std::string serialized_parameters;

		if(this->filtering & GPU_SAMPLER_FILTERING_LINEAR)
		{
			serialized_parameters += "linear-filter_";
		}

		if(this->filtering & GPU_SAMPLER_FILTERING_MIPMAP)
		{
			serialized_parameters += "mipmap_";
		}

		if(this->filtering & GPU_SAMPLER_FILTERING_ANISOTROPIC)
		{
			serialized_parameters += "anisotropic_";
		}

		switch(this->extend_x)
		{
		case GPU_SAMPLER_EXTEND_MODE_EXTEND:
			serialized_parameters += "extend-x_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_REPEAT:
			serialized_parameters += "repeat-x_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_MIRRORED_REPEAT:
			serialized_parameters += "mirrored-repeat-x_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_CLAMP_TO_BORDER:
			serialized_parameters += "clamp-to-border-x_";
			break;
		default:
			BLI_assert_unreachable();
		}

		switch(this->extend_yz)
		{
		case GPU_SAMPLER_EXTEND_MODE_EXTEND:
			serialized_parameters += "extend-y_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_REPEAT:
			serialized_parameters += "repeat-y_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_MIRRORED_REPEAT:
			serialized_parameters += "mirrored-repeat-y_";
			break;
		case GPU_SAMPLER_EXTEND_MODE_CLAMP_TO_BORDER:
			serialized_parameters += "clamp-to-border-y_";
			break;
		default:
			BLI_assert_unreachable();
		}

		switch(this->extend_yz)
		{
		case GPU_SAMPLER_EXTEND_MODE_EXTEND:
			serialized_parameters += "extend-z";
			break;
		case GPU_SAMPLER_EXTEND_MODE_REPEAT:
			serialized_parameters += "repeat-z";
			break;
		case GPU_SAMPLER_EXTEND_MODE_MIRRORED_REPEAT:
			serialized_parameters += "mirrored-repeat-z";
			break;
		case GPU_SAMPLER_EXTEND_MODE_CLAMP_TO_BORDER:
			serialized_parameters += "clamp-to-border-z";
			break;
		default:
			BLI_assert_unreachable();
		}

		return serialized_parameters;
	}

	bool operator==(GPUSamplerState const& rhs) const
	{
		return this->filtering == rhs.filtering && this->extend_x == rhs.extend_x &&
			this->extend_yz == rhs.extend_yz && this->custom_type == rhs.custom_type &&
			this->type == rhs.type;
	}
#endif
} GPUSamplerState;


/**
* Types of texture internal storage. Defines how the data is stored inside the video memory.
* Be aware that some formats are not supported by render-buffers.
*/
typedef enum eGPUTextureFormat
{
	/* Formats texture & render-buffer. */

	GPU_RGBA8UI,
	GPU_RGBA8I,
	GPU_RGBA8,
	GPU_RGBA16UI,
	GPU_RGBA16I,
	GPU_RGBA16F,
	GPU_RGBA16,
	GPU_RGBA32UI,
	GPU_RGBA32I,
	GPU_RGBA32F,

	GPU_RG8UI,
	GPU_RG8I,
	GPU_RG8,
	GPU_RG16UI,
	GPU_RG16I,
	GPU_RG16F,
	GPU_RG16,
	GPU_RG32UI,
	GPU_RG32I,
	GPU_RG32F,

	GPU_R8UI,
	GPU_R8I,
	GPU_R8,
	GPU_R16UI,
	GPU_R16I,
	GPU_R16F,
	GPU_R16,
	GPU_R32UI,
	GPU_R32I,
	GPU_R32F,

	/* Special formats texture & render-buffer. */

	GPU_RGB10_A2,
	GPU_RGB10_A2UI,
	GPU_R11F_G11F_B10F,
	GPU_DEPTH32F_STENCIL8,
	GPU_DEPTH24_STENCIL8,
	GPU_SRGB8_A8,

	/* Texture only formats. */

	GPU_RGBA8_SNORM,
	GPU_RGBA16_SNORM,

	GPU_RGB8UI,
	GPU_RGB8I,
	GPU_RGB8,
	GPU_RGB8_SNORM,
	GPU_RGB16UI,
	GPU_RGB16I,
	GPU_RGB16F,
	GPU_RGB16,
	GPU_RGB16_SNORM,
	GPU_RGB32UI,
	GPU_RGB32I,
	GPU_RGB32F,

	GPU_RG8_SNORM,
	GPU_RG16_SNORM,

	GPU_R8_SNORM,
	GPU_R16_SNORM,

	/* Special formats, texture only. */
	GPU_SRGB8_A8_DXT1, /* BC1 */
	GPU_SRGB8_A8_DXT3, /* BC2 */
	GPU_SRGB8_A8_DXT5, /* BC3 */
	GPU_RGBA8_DXT1,    /* BC1 */
	GPU_RGBA8_DXT3,    /* BC2 */
	GPU_RGBA8_DXT5,    /* BC3 */
	GPU_SRGB8,
	GPU_RGB9_E5,
#if 0 /* TODO: Add support for them. */
	GPU_COMPRESSED_RG_RGTC2,
	GPU_COMPRESSED_SIGNED_RG_RGTC2,
	GPU_COMPRESSED_RED_RGTC1,
	GPU_COMPRESSED_SIGNED_RED_RGTC1,
#endif

	/* Depth Formats. */
	GPU_DEPTH_COMPONENT32F,
	GPU_DEPTH_COMPONENT24,
	GPU_DEPTH_COMPONENT16,
} eGPUTextureFormat;

#ifndef __cplusplus
/** Identical to GPUSamplerState::default_sampler for non C++ users. */
static const GPUSamplerState GPU_SAMPLER_DEFAULT = {GPU_SAMPLER_FILTERING_DEFAULT,
GPU_SAMPLER_EXTEND_MODE_EXTEND,
GPU_SAMPLER_EXTEND_MODE_EXTEND,
GPU_SAMPLER_CUSTOM_COMPARE,
GPU_SAMPLER_STATE_TYPE_PARAMETERS};
#endif
