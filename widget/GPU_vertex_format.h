#pragma once
#include "GPU_common.h"

#define GPU_VERT_ATTR_MAX_NAMES 6
#define GPU_VERT_ATTR_MAX_LEN 16
#define GPU_VERT_FORMAT_MAX_NAMES 63 /* More than enough, actual max is ~30. */
#define GPU_VERT_ATTR_NAMES_BUF_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct GPUVertAttr {
		uint fetch_mode : 2;
		uint comp_type : 3;
		/* 1 to 4 or 8 or 12 or 16 */
		uint comp_len : 5;
		/* size in bytes, 1 to 64 */
		uint sz : 7;
		/* from beginning of vertex, in bytes */
		uint offset : 11;
		/* up to GPU_VERT_ATTR_MAX_NAMES */
		uint name_len : 3;
		uint gl_comp_type;
		/* -- 8 Bytes -- */
		uchar names[GPU_VERT_ATTR_MAX_NAMES];
	}GPUVertAttr;

	typedef struct GPUVertFormat {
		/** 0 to 16 (GPU_VERT_ATTR_MAX_LEN). */
		uint attr_len : 5;
		/** Total count of active vertex attribute names. (max GPU_VERT_FORMAT_MAX_NAMES) */
		uint name_len : 6;
		/** Stride in bytes, 1 to 1024. */
		uint stride : 11;
		/** Has the format been packed. */
		uint packed : 1;
		/** Current offset in names[]. */
		uint name_offset : 8;
		/** Store each attribute in one contiguous buffer region. */
		uint deinterleaved : 1;

		GPUVertAttr attrs[GPU_VERT_ATTR_MAX_LEN];
		char names[GPU_VERT_ATTR_NAMES_BUF_LEN];
	} GPUVertFormat;

	typedef enum {
		GPU_COMP_I8,
		GPU_COMP_U8,
		GPU_COMP_I16,
		GPU_COMP_U16,
		GPU_COMP_I32,
		GPU_COMP_U32,

		GPU_COMP_F32,

		GPU_COMP_I10,
	} GPUVertCompType;

	typedef enum {
		GPU_FETCH_FLOAT,
		GPU_FETCH_INT,
		GPU_FETCH_INT_TO_FLOAT_UNIT, /* 127 (ubyte) -> 0.5 (and so on for other int types) */
		GPU_FETCH_INT_TO_FLOAT,      /* 127 (any int type) -> 127.0 */
	} GPUVertFetchMode;

	uchar copy_attr_name(GPUVertFormat* format, const char* name);
	void GPU_vertformat_copy(GPUVertFormat* dest, const GPUVertFormat* src);
	void GPU_vertformat_clear(GPUVertFormat*);


	uint GPU_vertformat_attr_add(GPUVertFormat* format, const char* name, GPUVertCompType comp_type, uint comp_len, GPUVertFetchMode fetch_mode);

	uint vertex_buffer_size(const GPUVertFormat* format, uint vertex_len);

	uint padding(uint offset, uint alignment);

	const char* GPU_vertformat_attr_name_get(const GPUVertFormat* format, const GPUVertAttr* attr, uint n_idx);

#ifdef __cplusplus
}
#endif