#ifndef __GPU_VERTEX_BUFFER_H__
#define __GPU_VERTEX_BUFFER_H__

#include "GPU_vertex_format.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	/* can be extended to support more types */
	GPU_USAGE_STREAM,
	GPU_USAGE_STATIC, /* do not keep data in memory */
	GPU_USAGE_DYNAMIC,
} GPUUsageType;

typedef struct GPUVertBuf {
	GPUVertFormat format;
	/** Number of verts we want to draw. */
	uint vertex_len;
	/** Number of verts data. */
	uint vertex_alloc;
	/** 0 indicates not yet allocated. */
	uint32_t vbo_id;
	/** Usage hint for GL optimisation. */
	uint usage : 2;
	/** Data has been touched and need to be reuploaded to GPU. */
	uint dirty : 1;
	unsigned char* data; /* NULL indicates data in VRAM (unmapped) */
} GPUVertBuf;

void GPU_vertformat_copy(GPUVertFormat* dest, const GPUVertFormat* src);

GPUVertBuf* GPU_vertbuf_create_with_format_ex(const GPUVertFormat*, GPUUsageType);

#define GPU_vertbuf_create_with_format(format) \
  GPU_vertbuf_create_with_format_ex(format, GPU_USAGE_STATIC)

void GPU_vertbuf_init(GPUVertBuf*, GPUUsageType);

GPUVertBuf* GPU_vertbuf_create(GPUUsageType);

void GPU_vertbuf_data_alloc(GPUVertBuf*, uint v_len);

#ifdef __cplusplus
}
#endif

#endif