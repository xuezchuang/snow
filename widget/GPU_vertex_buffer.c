#include "GPU_vertex_buffer.h"
#include <stdlib.h>
#include <string.h>
GPUVertBuf* GPU_vertbuf_create(GPUUsageType usage)
{
	//GPUVertBuf* verts = MEM_mallocN(sizeof(GPUVertBuf), "GPUVertBuf");
	GPUVertBuf* verts = malloc(sizeof(GPUVertBuf));
	GPU_vertbuf_init(verts, usage);
	return verts;
}

void GPU_vertbuf_init(GPUVertBuf* verts, GPUUsageType usage)
{
	memset(verts, 0, sizeof(GPUVertBuf));
	verts->usage = usage;
	verts->dirty = true;
}

GPUVertBuf* GPU_vertbuf_create_with_format_ex(const GPUVertFormat* format, GPUUsageType usage)
{
	GPUVertBuf* verts = GPU_vertbuf_create(usage);
	GPU_vertformat_copy(&verts->format, format);
	//if (!format->packed) {
	//	VertexFormat_pack(&verts->format);
	//}
	return verts;

	/* this function might seem redundant, but there is potential for memory savings here... */
	/* TODO: implement those memory savings */
}

/* create a new allocation, discarding any existing data */
void GPU_vertbuf_data_alloc(GPUVertBuf* verts, uint v_len)
{
	//GPUVertFormat* format = &verts->format;
	//if (!format->packed) {
	//	VertexFormat_pack(format);
	//}
	///* catch any unnecessary use */
	//assert(verts->vertex_alloc != v_len || verts->data == NULL);
	///* discard previous data if any */
	//if (verts->data) {
	//	MEM_freeN(verts->data);
	//}
	//uint new_size = vertex_buffer_size(&verts->format, v_len);
	//vbo_memory_usage += new_size - GPU_vertbuf_size_get(verts);
	//verts->dirty = true;
	//verts->vertex_len = verts->vertex_alloc = v_len;
	//verts->data = MEM_mallocN(sizeof(GLubyte) * GPU_vertbuf_size_get(verts), "GPUVertBuf data");
}