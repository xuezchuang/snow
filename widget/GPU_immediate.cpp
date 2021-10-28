#include "GPU_immediate.h"
#include <string.h>
#include "Matrices.h"
#include "vmath.h"
//#include "GPU_matrix.h"
//-------------------------------------------//
GPUImmediate::GPUImmediate()
{
	Init();
}

GPUImmediate::~GPUImmediate()
{}

void GPUImmediate::Init()
{
	glGenVertexArrays(1, &imm.vao_id);
	glBindVertexArray(imm.vao_id);

	glGenBuffers(1, &Imm_buffer_vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, Imm_buffer_vbo_id);
	glBufferData(GL_ARRAY_BUFFER, 4 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);
	buffer_size = 0;
	buffer_offset = 0;
}
//extern uint padding(uint offset, uint alignment);
//extern uint vertex_buffer_size(const GPUVertFormat* format, uint vertex_len);
void GPUImmediate::immBegin(GLuint prim_type, uint vertex_len)
{
	glBindBuffer(GL_ARRAY_BUFFER, Imm_buffer_vbo_id);
	const uint available_bytes = buffer_size - buffer_offset;
	const uint bytes_needed = vertex_buffer_size(&imm.vertex_format, vertex_len);
	bool recreate_buffer = false;
	if (bytes_needed > buffer_size)
	{
		recreate_buffer = true;
		buffer_size = bytes_needed;
	}

	if (recreate_buffer)
	{
		//glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_DYNAMIC_DRAW);
		//buffer_offset = 0;
	}
	else
	{
		const uint pre_padding = padding(buffer_offset, imm.vertex_format.stride);
		buffer_offset += pre_padding;
	}
	imm.vertex_len = vertex_len;
	imm.prim_type = prim_type;
	imm.buffer_bytes_mapped = bytes_needed;
	imm.vertex_data = (GLubyte*)glMapBufferRange(GL_ARRAY_BUFFER, buffer_offset, bytes_needed, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
}




void GPUImmediate::immBindProgram(eGPUBuiltinShader program)
{
	IGPUShader::Instance()->bindShader(program);
	GPUVertFormat* format = &imm.vertex_format;
	GPUVertAttr* a0 = &format->attrs[0];
	a0->offset = 0;
	uint offset = a0->sz;
	for (uint a_idx = 1; a_idx < format->attr_len; a_idx++) {
		GPUVertAttr* a = &format->attrs[a_idx];
		a->offset = offset;
		offset += a->sz;
	}
	format->stride = offset;
	imm.shader_interface = IGPUShader::Instance()->GetShader(program)->program;
}

void GPUImmediate::immDrawSetup(void)
{
	/* set up VAO -- can be done during Begin or End really */
	glBindVertexArray(imm.vao_id);
	if (imm.vertex_format.attr_len > 1)
		glEnableVertexAttribArray(1);
	else
		glDisableVertexAttribArray(1);
	const uint stride = imm.vertex_format.stride;

	for (uint a_idx = 0; a_idx < imm.vertex_format.attr_len; a_idx++) {
		const GPUVertAttr* a = &imm.vertex_format.attrs[a_idx];

		const uint offset = buffer_offset + a->offset;
		const GLvoid* pointer = (const GLubyte*)0 + offset;

		const uint loc = a_idx;

		glVertexAttribPointer(loc, a->comp_len, a->gl_comp_type, GL_FALSE, stride, pointer);
	}

}

void GPUImmediate::immEnd()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	if (imm.vertex_len > 0) {
		immDrawSetup();
		glDrawArrays(imm.prim_type, 0, imm.vertex_len);
		buffer_offset += imm.buffer_bytes_mapped;
	}
}
void GPUImmediate::immEndVertex(void)
{
	//imm.vertex_idx++;
	imm.vertex_data += imm.vertex_format.stride;
}

void GPUImmediate::immAttr3f(uint attr_id, float x, float y, float z)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];
	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
	data[2] = z;
}

void GPUImmediate::immAttr2f(uint attr_id, float x, float y)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];

	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
}

void GPUImmediate::immVertex2f(uint attr_id, float x, float y)
{
	immAttr2f(attr_id, x, y);
	immEndVertex();
}

GPUVertFormat* GPUImmediate::immVertexFormat()
{
	GPU_vertformat_clear(&imm.vertex_format);
	return &imm.vertex_format;
}
void GPUImmediate::immUniformColor3f(float r, float g, float b)
{
	GLuint uniform_loc = glGetUniformLocation(imm.shader_interface, "color");
	const float scale = 1.0f;// / 255.0f;`
	glUniform4f(uniform_loc, r * scale, g * scale, b * scale, 1.0);
}

void GPUImmediate::matrix_projection_set(const float m[4][4])
{
	GLuint uniform_loc = glGetUniformLocation(imm.shader_interface, "projection_matrix");
	glUniformMatrix4fv(uniform_loc,1, GL_FALSE, (GLfloat*)m);
}
void GPUImmediate::matrix_model_view_set(const float m[4][4])
{
	GLuint uniform_loc = glGetUniformLocation(imm.shader_interface, "model_matrix");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (GLfloat*)m);
}
