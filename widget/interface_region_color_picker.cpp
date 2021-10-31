#include "interface_region_color_picker.h"
#include "gl/glew.h"
#include <string.h>
#include "vmath.h"
#include "GPU_vertex_format.h"
#include <windows.h>
#include "IFrameApi.h"
#include "GPU_immediate.h"
#include "math_matrix.h"

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
float len_squared_v2(const float v[2])
{
	return v[0] * v[0] + v[1] * v[1];
}

static void ui_hsvcircle_pos_from_vals(const rctf* rect ,const float* hsv, float* r_xpos, float* r_ypos)
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
static void ui_hsvcircle_vals_from_pos(const rctf* m_rect, const float mx, const float my, float* r_val_rad, float* r_val_dist)
{
	/* duplication of code... well, simple is better now */
	const float centx = float((m_rect->xmin + m_rect->xmax) / 2.0);
	const float centy = float((m_rect->ymin + m_rect->ymax) / 2.0);
	const float radius = (float)min(m_rect->xmax - m_rect->xmin, m_rect->ymax - m_rect->ymin) / 2.0f;
	const float m_delta[2] = { mx - centx, my - centy };
	const float dist_sq = len_squared_v2(m_delta);

	*r_val_dist = (dist_sq < (radius* radius)) ? sqrtf(dist_sq) / radius : 1.0f;
	*r_val_rad = atan2f(m_delta[0], m_delta[1]) / (2.0f * (float)M_PI) + 0.5f;
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

#pragma endregion 
//-------------------------------------------//
IRColorPicker::IRColorPicker()
{
	m_hsv[0] = 0.0163f; m_hsv[1] = 0.9875f; m_hsv[2] = 1.0f;
}

IRColorPicker::~IRColorPicker()
{}


void IRColorPicker::draw()
{
	const int tot = 64;
	GPUVertFormat* format = m_pImmediate->immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2, GPU_FETCH_FLOAT);
	uint color = GPU_vertformat_attr_add(format, "color", GPU_COMP_F32, 3, GPU_FETCH_FLOAT);
	m_pImmediate->immBindProgram(GPU_SHADER_2D_SMOOTH_COLOR);
	glDisable(GL_DEPTH_TEST);
	float viewport_size[4];
	glGetFloatv(GL_VIEWPORT, viewport_size);
	vmath::mat4 projection_matrix(vmath::Orthogonal(0.0f, viewport_size[2], 0.0f, viewport_size[3], -500.0f, 500.0f));
	vmath::mat4 model_matrix = vmath::translation(0.0f, 0.0f, 0.0f);
	model_matrix *= vmath::scale(1.0f);
	{
		m_pImmediate->matrix_projection_set((float(*)[4])(float*)projection_matrix);
		m_pImmediate->matrix_model_view_set((float(*)[4])(float*)model_matrix);
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
	m_pImmediate->immBegin(GL_TRIANGLE_FAN, tot + 2);
	
	float/* rgb[3],*/ hsv[3], rgb_center[3];
	const float centx = BLI_rcti_cent_x_fl(&m_rect);
	const float centy = BLI_rcti_cent_y_fl(&m_rect);
	hsv[0] = m_hsv[0]; hsv[1] = m_hsv[1]; hsv[2] = m_hsv[2];
	rgb_center[0] = 1; rgb_center[1] = 1; rgb_center[2] = 1;

	m_pImmediate->immAttr3f(color, rgb_center[0], rgb_center[1], rgb_center[2]);
	m_pImmediate->immVertex2f(pos, centx, centy);

	float ang = 0.0f;
	const float radstep = 2.0f * (float)M_PI / (float)tot;
	const float radius = (float)min(m_rect.xmax - m_rect.xmin, m_rect.ymax - m_rect.ymin) / 2.0f;
	for (int a = 0; a <= tot; a++, ang += radstep) {
		float si = sinf(ang);
		float co = cosf(ang);
		float hsv_ang[3];
		float rgb_ang[3];

		ui_hsvcircle_vals_from_pos(&m_rect, centx + co * radius, centy + si * radius, hsv_ang, hsv_ang + 1);
		hsv_ang[2] = hsv[2];

		hsv_to_rgb_v(hsv_ang, rgb_ang);
		//ui_color_picker_to_scene_linear_space(but, rgb_ang);

		//if (!is_color_gamma) {
		//	ui_block_cm_to_display_space_v3(but->block, rgb_ang);
		//}

		m_pImmediate->immAttr3f(color, rgb_ang[0], rgb_ang[1], rgb_ang[2]);
		m_pImmediate->immVertex2f(pos, centx + co * radius, centy + si * radius);
	}

	m_pImmediate->immEnd();

	glUseProgram(0);
	{
		GPUVertFormat* format = m_pImmediate->immVertexFormat();
		uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2, GPU_FETCH_FLOAT);
		m_pImmediate->immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
		m_pImmediate->matrix_projection_set((float(*)[4])(float*)projection_matrix);
		m_pImmediate->matrix_model_view_set((float(*)[4])(float*)model_matrix);
		//glDisableVertexAttribArray(1);
		
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		m_pImmediate->immUniformColor3f(0.1f, 0.1f, 0.2f);
		imm_draw_circle(GL_LINE_LOOP, pos, centx, centy, radius, radius, 200);
	}
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	// 
	float x, y;
	ui_hsvcircle_pos_from_vals(&m_rect, hsv, &x, &y);
	ui_hsv_cursor(x, y);
	glUseProgram(0);
}

//void IRColorPicker::set_radio(float radio)
//{
//	m_radio = radio;
//}
bool IRColorPicker::update(float x, float y)
{
	if(x > m_rect.xmin && x < m_rect.xmax && y > m_rect.ymin && y <m_rect.ymax)
	{
		ui_hsvcircle_vals_from_pos(&m_rect, x, y, &m_hsv[0], &m_hsv[1]);
		return true;
	}
	return false;
}

void IRColorPicker::getcolor(float rgbcolor[4])
{
	hsv_to_rgb_v(m_hsv, rgbcolor);
	rgbcolor[3] = 1.0;
}

void IRColorPicker::ui_hsv_cursor(float x, float y)
{
	GPUVertFormat* format = m_pImmediate->immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2, GPU_FETCH_FLOAT);
	m_pImmediate->immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
	float viewport_size[4];
	glGetFloatv(GL_VIEWPORT, viewport_size);
	vmath::mat4 projection_matrix(vmath::Orthogonal(0.0f, viewport_size[2], 0.0f, viewport_size[3], -500.0f, 500.0f));
	vmath::mat4 model_matrix = vmath::translation(0.0f, 0.0f, 0.0f);
	m_pImmediate->matrix_projection_set((float(*)[4])(float*)projection_matrix);
	m_pImmediate->matrix_model_view_set((float(*)[4])(float*)model_matrix);
	m_pImmediate->immUniformColor3f(1, 1, 1);
	imm_draw_circle(GL_TRIANGLE_FAN, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 8);

	//immUnbindProgram();
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	m_pImmediate->immUniformColor3f(0, 0, 0);
	//imm_draw_circle(GL_TRIANGLE_FAN, pos, x,y, 0.03f, 0.03f, 8);
	imm_draw_circle(GL_LINE_LOOP, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 12);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

void IRColorPicker::imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments)
{
	m_pImmediate->immBegin(prim_type, nsegments);
	for (int i = 0; i < nsegments; i++) {
		const float angle = (float)(2 * M_PI) * ((float)i / (float)nsegments);
		m_pImmediate->immVertex2f(shdr_pos, x + (rad_x * cosf(angle)), y + (rad_y * sinf(angle)));
	}
	m_pImmediate->immEnd();
}

