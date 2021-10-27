
#include "GPU_batch.h"

void GPU_batch_program_use_begin(GPUBatch* batch)
{
	glUseProgram(batch->program);
}

void GPU_batch_program_use_end(GPUBatch* batch)
{
	glUseProgram(0);
}

void GPU_batch_bind(GPUBatch* batch)
{
	glBindVertexArray(batch->vao_id);
}

static void* elem_offset(const GPUIndexBuf* el, int v_first)
{
#if GPU_TRACK_INDEX_RANGE
	if (el->index_type == GPU_INDEX_U16) {
		return (GLushort*)0 + v_first + el->index_start;
	}
#endif
	return (GLuint*)0 + v_first + el->index_start;
}

#if GPU_TRACK_INDEX_RANGE
#  define BASE_INDEX(el) ((el)->base_index)
#  define INDEX_TYPE(el) ((el)->gl_index_type)
#else
#  define BASE_INDEX(el) 0
#  define INDEX_TYPE(el) GL_UNSIGNED_INT
#endif

#if TRUST_NO_ONE
#  define GET_UNIFORM \
    const GPUShaderInput *uniform = GPU_shaderinterface_uniform(batch->interface, name); \
    assert(uniform);
#else
#  define GET_UNIFORM \
    const GPUShaderInput *uniform = GPU_shaderinterface_uniform(batch->interface, name);
#endif

void GPU_batch_draw_advanced(GPUBatch* batch, int v_first, int v_count, int i_first, int i_count)
{
	if (v_count == 0) {
		//v_count = (batch->elem) ? batch->elem->index_len : batch->verts[0]->vertex_len;
		v_count = batch->elem->index_len;
	}
	if (i_count == 0) {
		i_count = (batch->inst[0]) ? batch->inst[0]->vertex_len : 1;
	}
	if (batch->elem) {
		const GPUIndexBuf* el = batch->elem;
		GLenum index_type = INDEX_TYPE(el);
		GLint base_index = BASE_INDEX(el);
		void* v_first_ofs = elem_offset(el, v_first);
		glDrawElementsInstancedBaseVertexBaseInstance(batch->gl_prim_type, v_count, index_type, v_first_ofs, i_count, base_index, i_first);
	}
}

void GPU_batch_draw(GPUBatch* batch) {
	GPU_batch_program_use_begin(batch);
	GPU_batch_bind(batch);
	GPU_batch_draw_advanced(batch, 0, 0, 0, 0);

	GPU_batch_program_use_end(batch);
}

void GPU_batch_uniform_4fv_array(GPUBatch* batch,const char* name,const int len,const float* data)
{
	int32_t location = glGetUniformLocation(batch->program, name);
	glUniform4fv(location, len, data);
}
