#ifndef __GPU_BATCH_H__
#define __GPU_BATCH_H__
#include "GPU_common.h"
#include "interface_intern.h"
#include "GPU_vertex_buffer.h"
#ifdef __cplusplus
extern "C" {
	int a = 3;
#endif

typedef struct GPUIndexBuf {

	uint index_start;
	uint index_len;
	bool is_subrange;
	uint32_t ibo_id; /* 0 indicates not yet sent to VRAM */
	union {
		void* data;              /* non-NULL indicates not yet sent to VRAM */
		struct GPUIndexBuf* src; /* if is_subrange is true, this is the source buffer. */
	};
} GPUIndexBuf;

typedef struct UserDef {
	/** Runtime, line width and point size based on DPI. */
	float pixelsize;

} UserDef;



#define GPU_BATCH_VBO_MAX_LEN 6
#define GPU_BATCH_INST_VBO_MAX_LEN 2

typedef struct GPUBatch {
	/* geometry */

	/** verts[0] is required, others can be NULL */
	GPUVertBuf* verts[GPU_BATCH_VBO_MAX_LEN];
	/** Instance attributes. */
	GPUVertBuf* inst[GPU_BATCH_INST_VBO_MAX_LEN];
	/** NULL if element list not needed */
	GPUIndexBuf* elem;
	uint32_t gl_prim_type;

	/* cached values (avoid dereferencing later) */
	uint32_t vao_id;
	uint32_t program;

}GPUBatch;




void GPU_batch_draw(GPUBatch*);
void GPU_batch_uniform_4fv_array(GPUBatch*, const char* name, const int len, const float* data);

#ifdef __cplusplus
}
#endif

#endif