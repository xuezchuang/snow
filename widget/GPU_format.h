#pragma once

typedef unsigned int uint;
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
	//uchar names[6];
} GPUVertAttr;
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
	GPUVertAttr attrs[16];

}GPUVertFormat;



