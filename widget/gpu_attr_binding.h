#pragma once
#include "GPU_common.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct GPUAttrBinding {
		/** Store 4 bits for each of the 16 attributes. */
		uint64_t loc_bits;
		/** 1 bit for each attribute. */
		uint16_t enabled_bits;
	} GPUAttrBinding;

	void AttrBinding_clear(GPUAttrBinding* binding);

	void get_attr_locations(const GPUVertFormat* format,GPUAttrBinding* binding);
	uint read_attr_location(const GPUAttrBinding* binding, uint a_idx);

#ifdef __cplusplus
}
#endif