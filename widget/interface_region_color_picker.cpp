#include "interface_region_color_picker.h"
#include "gl/glew.h"
#include <string.h>
#include "Matrices.h"
#include "vmath.h"
#include "GPU_vertex_format.h"
#include <windows.h>
#include "IFrameApi.h"
#include "GPU_immediate.h"
#pragma region
static float clamp_f(float value, float min, float max)
{
	if (value > max) {
		return max;
	}
	else if (value < min) {
		return min;
	}
	return value;
}

static void ui_hsvcircle_pos_from_vals(const rctf* rect, const float* hsv, float* r_xpos, float* r_ypos)
{
	float radius = (float)min(rect->xmax - rect->xmin, rect->ymax - rect->ymin) / 2.0f;
	const float centx = float((rect->xmin + rect->xmax) / 2.0);
	const float centy = float((rect->ymin + rect->ymax) / 2.0);
	const float ang = 2.0f * (float)M_PI * hsv[0] + (float)M_PI_2;
	float radius_t = hsv[1];

	radius = clamp_f(radius_t, 0.0f, 1.0f) * radius;
	*r_xpos = centx + cosf(-ang) * radius;
	*r_ypos = centy + sinf(-ang) * radius;
}


GLenum convert_comp_type_to_gl(GPUVertCompType type)
{
	static const GLenum table[] = {
		GL_BYTE,
		GL_UNSIGNED_BYTE,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_INT,
		GL_UNSIGNED_INT,

		GL_FLOAT,

		GL_INT_2_10_10_10_REV,
	};
	return table[type];
}
uint attr_sz(const GPUVertAttr* a)
{
	if (a->comp_type == GPU_COMP_I10) {
		return 4; /* always packed as 10_10_10_2 */
	}
	const GLubyte sizes[] = { 1, 1, 2, 2, 4, 4, 4 };
	uint usize = sizes[a->comp_type];

	return a->comp_len * usize;
}

uint GPU_vertformat_attr_add(GPUVertFormat* format, const char* name, GPUVertCompType comp_type, uint comp_len)
{
	format->name_len++; /* multiname support */

	const uint attr_id = format->attr_len++;
	GPUVertAttr* attr = &format->attrs[attr_id];

	//attr->names[attr->name_len++] = copy_attr_name(format, name);
	attr->comp_type = comp_type;
	attr->gl_comp_type = convert_comp_type_to_gl(comp_type);
	attr->comp_len = (comp_type == GPU_COMP_I10) ? 4 : comp_len; /* system needs 10_10_10_2 to be 4 or BGRA */
	attr->sz = attr_sz(attr);
	attr->offset = 0; /* offsets & stride are calculated later (during pack) */
	//attr->fetch_mode = fetch_mode;
	return attr_id;
}

float len_squared_v2(const float v[2])
{
	return v[0] * v[0] + v[1] * v[1];
}

void ui_hsvcircle_vals_from_pos(const rctf* rect, const float mx, const float my, float* r_val_rad, float* r_val_dist)
{
	/* duplication of code... well, simple is better now */
	const float centx = float((rect->xmin + rect->xmax) / 2.0);
	const float centy = float((rect->ymin + rect->ymax) / 2.0);
	const float radius = (float)min(rect->xmax - rect->xmin, rect->ymax - rect->ymin) / 2.0f;
	const float m_delta[2] = { mx - centx, my - centy };
	const float dist_sq = len_squared_v2(m_delta);

	*r_val_dist = (dist_sq < (radius* radius)) ? sqrtf(dist_sq) / radius : 1.0f;
	*r_val_rad = atan2f(m_delta[0], m_delta[1]) / (2.0f * (float)M_PI) + 0.5f;
}


#define CLAMP(a, b, c) \
  { \
    if ((a) < (b)) \
      (a) = (b); \
    else if ((a) > (c)) \
      (a) = (c); \
  } 
void hsv_to_rgb(float h, float s, float v, float* r_r, float* r_g, float* r_b)
{
	float nr, ng, nb;

	nr = fabsf(h * 6.0f - 3.0f) - 1.0f;
	ng = 2.0f - fabsf(h * 6.0f - 2.0f);
	nb = 2.0f - fabsf(h * 6.0f - 4.0f);

	CLAMP(nr, 0.0f, 1.0f);
	CLAMP(nb, 0.0f, 1.0f);
	CLAMP(ng, 0.0f, 1.0f);

	*r_r = ((nr - 1.0f) * s + 1.0f) * v;
	*r_g = ((ng - 1.0f) * s + 1.0f) * v;
	*r_b = ((nb - 1.0f) * s + 1.0f) * v;
}

void hsv_to_rgb_v(const float hsv[3], float r_rgb[3])
{
	hsv_to_rgb(hsv[0], hsv[1], hsv[2], &r_rgb[0], &r_rgb[1], &r_rgb[2]);
}

void rgb_to_hsv(float r, float g, float b, float* r_h, float* r_s, float* r_v)
{
	float k = 0.0f;
	float chroma;
	float min_gb;

#define SWAP(type, a, b) \
  { \
    type sw_ap; \
    sw_ap = (a); \
    (a) = (b); \
    (b) = sw_ap; \
 }

	if (g < b) {
		SWAP(float, g, b);
		k = -1.0f;
	}
	min_gb = b;
	if (r < g) {
		SWAP(float, r, g);
		k = -2.0f / 6.0f - k;
		min_gb = min(g, b);
	}

	chroma = r - min_gb;

	*r_h = fabsf(k + (g - b) / (6.0f * chroma + 1e-20f));
	*r_s = chroma / (r + 1e-20f);
	*r_v = r;
}


void imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments)
{
	IFrameAPI::Instance()->Immediate()->immBegin(prim_type, nsegments);
	for (int i = 0; i < nsegments; i++) {
		const float angle = (float)(2 * M_PI) * ((float)i / (float)nsegments);
		IFrameAPI::Instance()->Immediate()->immVertex2f(shdr_pos, x + (rad_x * cosf(angle)), y + (rad_y * sinf(angle)));
	}
	IFrameAPI::Instance()->Immediate()->immEnd();
}


#pragma endregion 
//-------------------------------------------//
IRColorPicker::IRColorPicker()
{

}

IRColorPicker::~IRColorPicker()
{}

#include "math_matrix.h"
void IRColorPicker::DrawHSVCIRCLE(const rctf* rect)
{
	const int tot = 64;
	GPUVertFormat* format = IFrameAPI::Instance()->Immediate()->immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
	uint color = GPU_vertformat_attr_add(format, "color", GPU_COMP_F32, 3);
	IFrameAPI::Instance()->Immediate()->immBindProgram(GPU_SHADER_2D_SMOOTH_COLOR);
	{
		//GLuint pos = glGetAttribLocation(GPU_SHADER_2D_UNIFORM_COLOR, "position");
		GLuint pos = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "pos");
		GLuint color = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "color");

		glEnableVertexAttribArray(pos);
		glEnableVertexAttribArray(color);

		float viewport_size[4];
		glGetFloatv(GL_VIEWPORT, viewport_size);
		vmath::mat4 projection_matrix(vmath::Orthogonal(0.0f, viewport_size[2], 0.0f, viewport_size[3], -500.0f, 500.0f));
		vmath::mat4 model_matrix = vmath::translation(120.0f, viewport_size[3] - 120.0f, 0.0f);
		model_matrix *= vmath::scale(100.0f);
		IFrameAPI::Instance()->Immediate()->matrix_projection_set((float(*)[4])(float*)projection_matrix);
		IFrameAPI::Instance()->Immediate()->matrix_model_view_set((float(*)[4])(float*)model_matrix);
		////Test
		double temp[4] = { 0.0f,0.0f,0.0f,1.0f };
		double r[4];
		mul_v4d_m4v4d(r, (float(*)[4])(float*)model_matrix, temp);
		mul_v4d_m4v4d(temp, (float(*)[4])(float*)projection_matrix, r);
		float viewinv[4][4];
		invert_m4_m4(viewinv, (float(*)[4])(float*)model_matrix);
		//double temp2[4] = { 12,0.0f,0.0f,1.0f };
		mul_v4d_m4v4d(temp, viewinv, r);
	}
	


	
	IFrameAPI::Instance()->Immediate()->immBegin(GL_TRIANGLE_FAN, tot + 2);

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH);

	float/* rgb[3],*/ hsv[3], rgb_center[3];
	const float centx = 0.0;
	const float centy = 0.0;
	//hsv[0] = 0.08f; hsv[1] = 1.0f; hsv[2] = 1.0f;
	hsv[0] = 0.0163f; hsv[1] = 0.9875f; hsv[2] = 1.0f;
	rgb_center[0] = 1; rgb_center[1] = 1; rgb_center[2] = 1;

	IFrameAPI::Instance()->Immediate()->immAttr3f(color, rgb_center[0], rgb_center[1], rgb_center[2]);
	IFrameAPI::Instance()->Immediate()->immVertex2f(pos, centx, centy);

	float ang = 0.0f;
	const float radstep = 2.0f * (float)M_PI / (float)tot;
	const float radius = (float)min(rect->xmax - rect->xmin, rect->ymax - rect->ymin) / 2.0f;
	for (int a = 0; a <= tot; a++, ang += radstep) {
		float si = sinf(ang);
		float co = cosf(ang);
		float hsv_ang[3];
		float rgb_ang[3];

		ui_hsvcircle_vals_from_pos(rect, centx + co * radius, centy + si * radius, hsv_ang, hsv_ang + 1);
		hsv_ang[2] = hsv[2];

		hsv_to_rgb_v(hsv_ang, rgb_ang);
		//ui_color_picker_to_scene_linear_space(but, rgb_ang);

		//if (!is_color_gamma) {
		//	ui_block_cm_to_display_space_v3(but->block, rgb_ang);
		//}

		IFrameAPI::Instance()->Immediate()->immAttr3f(color, rgb_ang[0], rgb_ang[1], rgb_ang[2]);
		IFrameAPI::Instance()->Immediate()->immVertex2f(pos, centx + co * radius, centy + si * radius);
	}

	IFrameAPI::Instance()->Immediate()->immEnd();

	glUseProgram(0);
	{
		GPUVertFormat* format = IFrameAPI::Instance()->Immediate()->immVertexFormat();
		uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
		//glDisableVertexAttribArray(1);
		IFrameAPI::Instance()->Immediate()->immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		IFrameAPI::Instance()->Immediate()->immUniformColor3f(0.1f, 0.1f, 0.2f);
		imm_draw_circle(GL_LINE_LOOP, pos, centx, centy, radius, radius, 200);
	}
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	// 
	float x, y;
	ui_hsvcircle_pos_from_vals(rect, hsv, &x, &y);
	ui_hsv_cursor(x, y);
	glUseProgram(0);
}


void IRColorPicker::ui_hsv_cursor(float x, float y)
{
	GPUVertFormat* format = IFrameAPI::Instance()->Immediate()->immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
	IFrameAPI::Instance()->Immediate()->immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
	IFrameAPI::Instance()->Immediate()->immUniformColor3f(1, 1, 1);
	imm_draw_circle(GL_TRIANGLE_FAN, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 8);

	//immUnbindProgram();
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	IFrameAPI::Instance()->Immediate()->immUniformColor3f(0, 0, 0);
	//imm_draw_circle(GL_TRIANGLE_FAN, pos, x,y, 0.03f, 0.03f, 8);
	imm_draw_circle(GL_LINE_LOOP, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 12);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}