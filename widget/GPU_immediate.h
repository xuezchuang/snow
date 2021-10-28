#pragma once
#include "interface_intern.h"
#include "GPU_vertex_format.h"
#include "GPUShader.h"
typedef struct {
	GLuint shader_interface;
	uint vertex_len;
	uint buffer_bytes_mapped;
	GLuint vao_id;
	GLuint prim_type;
	GPUVertFormat vertex_format;
	GLubyte* vertex_data;
} Immediate;

class GPUImmediate
{
public:
	GPUImmediate();
	~GPUImmediate();
	void immBegin(GLuint prim_type, uint vertex_len);
	void immVertex2f(uint attr_id, float x, float y);
	void immEnd();
	void immBindProgram(eGPUBuiltinShader program);
	void immDrawSetup(void);
	
	void immEndVertex(void);
	void immAttr3f(uint attr_id, float x, float y, float z);
	void immAttr2f(uint attr_id, float x, float y);
	GPUVertFormat* immVertexFormat();
	void immUniformColor3f(float r, float g, float b);
	void matrix_projection_set(const float m[4][4]);
	void matrix_model_view_set(const float m[4][4]);
private:
	void Init();
	Immediate imm;
	GLuint Imm_buffer_vbo_id;
	uint buffer_size;
	uint buffer_offset;
};