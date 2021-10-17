/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "vapp.h"
#include "vutils.h"

#include "vmath.h"

#include "LoadShaders.h"

#include <stdio.h>
#include "interface_intern.h"
#include <assert.h>
#include "Matrices.h"
using namespace vmath;
UserDef U;
// Define USE_PRIMITIVE_RESTART to 0 to use two separate draw commands
#define USE_PRIMITIVE_RESTART 1
#pragma region gpuvertattr

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
typedef struct {
	GLuint shader_interface;
	uint vertex_len;
	uint buffer_bytes_mapped;
	GLuint vao_id;
	GLuint prim_type;
	GPUVertFormat vertex_format;
    GLubyte* vertex_data;
} Immediate;
#pragma endregion gpuvertattr
BEGIN_APP_DECLARATION(DrawCommandExample)
// Override functions from base class
virtual void Initialize(const char* title);
virtual void Display(bool auto_redraw);
virtual void Finalize(void);
virtual void Reshape(int width, int height);
virtual void keyboardCB(unsigned char key, int x, int y);
virtual void mouse(int button, int state, int x, int y);
//virtual void motion(int x, int y);
void DrawRect();
bool bttt = false;
// Member variables
float aspect;
GLuint render_prog;

GLuint vao[1];
GLuint vbo[1];
GLuint ebo[1];

GLint modelview_matrix_loc;
GLint project_matrix_loc;
GLint parameters_loc;
GLint test_loc;
GLint vcolor_loc;
int _ttx, _tty;
rctf m_CrupRect;
bool mouseLeftDown = false;
uiWidgetBaseParameters widgetParam;
//
GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;
#pragma region hsvcircle
    GLuint GPU_SHADER_2D_SMOOTH_COLOR;
	GLuint GPU_SHADER_2D_UNIFORM_COLOR;
    GLuint Imm_buffer_vbo_id;
    uint buffer_size;
    uint buffer_offset;
	void DrawHSVCIRCLE(const rctf* rect);
    //

	

    Immediate imm;
    //
    uint GPU_vertformat_attr_add(GPUVertFormat* format, const char* name, GPUVertCompType comp_type, uint comp_len);
	GLenum convert_comp_type_to_gl(GPUVertCompType type);
	uint attr_sz(const GPUVertAttr* a);
    void GPU_vertformat_clear(GPUVertFormat* format);
    GPUVertFormat* immVertexFormat();
	void immBindProgram(GLuint program);
    void immBegin(GLuint prim_type, uint vertex_len);
    void immEnd();
    void immDrawSetup();
    void immEndVertex();
    void immAttr3f(uint attr_id, float x, float y, float z);
    void immVertex2f(uint attr_id, float x, float y);
    void immAttr2f(uint attr_id, float x, float y);
	//
	float len_squared_v2(const float v[2]);
	void ui_hsvcircle_vals_from_pos(const rctf* rect, const float mx, const float my, float* r_val_rad, float* r_val_dist);
	void hsv_to_rgb_v(const float hsv[3], float r_rgb[3]);
	void hsv_to_rgb(float h, float s, float v, float* r_r, float* r_g, float* r_b);
	void ui_hsv_cursor(float x, float y);
	void imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments);
	void immUniformColor3f(float r, float g, float b);
#pragma endregion hsvcircle
//
END_APP_DECLARATION()

DEFINE_APP(DrawCommandExample, "Drawing Commands Example")

void DrawCommandExample::Initialize(const char* title)
{
    base::Initialize(title);

    static ShaderInfo shader_info[] =
    {
        //{ GL_VERTEX_SHADER, "./Shader/primitive_restart.vs.glsl" },
        //{ GL_FRAGMENT_SHADER, "./Shader/primitive_restart.fs.glsl" },
		{ GL_VERTEX_SHADER, "./Shader/gpu_shader_2D_widget_base_vert.glsl" },
		{ GL_FRAGMENT_SHADER, "./Shader/gpu_shader_2D_widget_base_frag.glsl" },
		{ GL_NONE, NULL }
	};

    render_prog = LoadShaders(shader_info);

    glUseProgram(render_prog);

    // "model_matrix" is actually an array of 4 matrices
    modelview_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
    project_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
    parameters_loc = glGetUniformLocation(render_prog, "parameters");
	vcolor_loc = glGetUniformLocation(render_prog, "vcolor");
    test_loc = glGetUniformLocation(render_prog, "btest");
	 //Indices for the triangle strips
	static const GLushort vertex_indices[] =
	{
		0, 1,2,3
	};
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
    //
	{
		static ShaderInfo GPU_SHADER_2D_SMOOTH_COLOR_info[] =
		{
			{ GL_VERTEX_SHADER, "./Shader/gpu_shader_2D_smooth_color_vert.glsl" },
			{ GL_FRAGMENT_SHADER, "./Shader/gpu_shader_2D_smooth_color_frag.glsl" },
			//{  GL_VERTEX_SHADER, "./Shader/primitive_restart.vs.glsl" },
			//{  GL_FRAGMENT_SHADER, "./Shader/primitive_restart.fs.glsl" },
			{ GL_NONE, NULL }
		};
		GPU_SHADER_2D_SMOOTH_COLOR = LoadShaders(GPU_SHADER_2D_SMOOTH_COLOR_info);
		static ShaderInfo GPU_SHADER_2D_UNIFORM_COLOR_info[] =
		{
			{ GL_VERTEX_SHADER, "./Shader/gpu_shader_uniform_color_vert.glsl" },
			{ GL_FRAGMENT_SHADER, "./Shader/gpu_shader_uniform_color_frag.glsl" },
			//{  GL_VERTEX_SHADER, "./Shader/primitive_restart.vs.glsl" },
			//{  GL_FRAGMENT_SHADER, "./Shader/primitive_restart.fs.glsl" },
			{ GL_NONE, NULL }
		};
		GPU_SHADER_2D_UNIFORM_COLOR = LoadShaders(GPU_SHADER_2D_UNIFORM_COLOR_info);
		

		

		//glUseProgram(GPU_SHADER_2D_UNIFORM_COLOR);
		glUseProgram(GPU_SHADER_2D_SMOOTH_COLOR);
		// Set up the vertex attributes
		glGenVertexArrays(1, &imm.vao_id);
		glBindVertexArray(imm.vao_id);

		glGenBuffers(1, &Imm_buffer_vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, Imm_buffer_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, 4 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);

		
		//GLuint pos = glGetAttribLocation(GPU_SHADER_2D_UNIFORM_COLOR, "position");
		GLuint pos = glGetAttribLocation(GPU_SHADER_2D_SMOOTH_COLOR, "pos");
		GLuint color = glGetAttribLocation(GPU_SHADER_2D_SMOOTH_COLOR, "color");

		//glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, NULL);
		//glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vertex_positions));
		glEnableVertexAttribArray(pos);
		glEnableVertexAttribArray(color);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
    buffer_size = 0;
    buffer_offset = 0;
    //glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	{
		U.pixelsize = 0.01f;
	}

}

/* keep in sync with shader */
#define MAX_WIDGET_BASE_BATCH 6
#define MAX_WIDGET_PARAMETERS 12

void DrawCommandExample::Display(bool auto_redraw)
{
	return;
    mat4 model_matrix;

 //   // Setup
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if(0)
	{
		// Activate simple shading program
		glUseProgram(render_prog);

		// Set up the model and projection matrix
		vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 400.0f));
		glUniformMatrix4fv(project_matrix_loc, 1, GL_FALSE, projection_matrix);
		// Set up for a glDrawElements call
		glBindVertexArray(vao[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
		//// Draw Arrays...
		model_matrix = vmath::translation(-3.0f, 0.0f, -5.0f);
		glUniformMatrix4fv(modelview_matrix_loc, 1, GL_FALSE, model_matrix);

		widgetParam.rect.xmin = -1.0f;
		widgetParam.rect.xmax = 1.f;
		widgetParam.rect.ymin = -1.f;
		widgetParam.rect.ymax = 1.f;
		glUniform4fv(parameters_loc, MAX_WIDGET_PARAMETERS, (float*)&widgetParam);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL, 1, 0, 0);
		//base::Display();
	}


    base::Display();
}

void DrawCommandExample::Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(render_prog);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
}

void DrawCommandExample::Reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    aspect = float(height) / float(width);
}

BLI_INLINE float BLI_rctf_size_x(const struct rctf* rct)
{
	return (rct->xmax - rct->xmin);
}
BLI_INLINE float BLI_rctf_size_y(const struct rctf* rct)
{
	return (rct->ymax - rct->ymin);
}

void DrawCommandExample::DrawRect()
{
	mat4 model_matrix;

	//   // Setup
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Activate simple shading program
	glUseProgram(render_prog);

	// Set up the model and projection matrix
	//vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 400.0));
	rctf viewplane;
	viewplane.xmin = -1.0f;		viewplane.xmax = 1.0f;
	viewplane.ymin = -aspect;	viewplane.ymax = aspect;
	double dscale = 5.0;
	{
		viewplane.xmin *= dscale;
		viewplane.xmax *= dscale;
		viewplane.ymin *= dscale;
		viewplane.ymax *= dscale;
	}
	vmath::mat4 projection_matrix(vmath::frustum(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, 1.0f, 400.0));
	//projection_matrix = vmath::Orthogonal(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, -500.0f, 500.0f);
	GLuint QueryIDs;
	glUniform4fv(vcolor_loc, 1, vec4(1.0f,0.0f,0.0f,1.0f));
	if(bttt)
		glUniform4fv(vcolor_loc, 1, vec4(0.0f, 0.0f, 1.0f, 1.0f));
	if (mouseLeftDown)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glGenQueries(1, &QueryIDs);
		glBeginQuery(GL_SAMPLES_PASSED, QueryIDs);
		
		//ViewPort(800, 560);
		//m_CrupRect.xmin = 80.0f;		m_CrupRect.xmax = 240.0f;
		//m_CrupRect.ymin = 300.0f;		m_CrupRect.ymax = 400.0f;
		//
		m_CrupRect.xmin = viewplane.xmin + (BLI_rctf_size_x(&viewplane) * (m_CrupRect.xmin / 800.0f));
		m_CrupRect.ymin = viewplane.ymin + (BLI_rctf_size_y(&viewplane) * (m_CrupRect.ymin / 560.0f));
		m_CrupRect.xmax = viewplane.xmin + (BLI_rctf_size_x(&viewplane) * (m_CrupRect.xmax / 800.0f));
		m_CrupRect.ymax = viewplane.ymin + (BLI_rctf_size_y(&viewplane) * (m_CrupRect.ymax / 560.0f));

		projection_matrix = vmath::Orthogonal(m_CrupRect.xmin, m_CrupRect.xmax, m_CrupRect.ymin, m_CrupRect.ymax, -400.0f, 400.0f);
		//计算出projection_matrix 后计算6个面对所有构件CPU剔除(简单快速)后,进行使用GPU裁剪
	}
	
	glUniformMatrix4fv(project_matrix_loc, 1, GL_FALSE, projection_matrix);
	// Set up for a glDrawElements call
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	//// Draw Arrays...
	//model_matrix = vmath::translation(0.0f, 0.0f, -5.0f);// -5.0f);
	//model_matrix = vmath::scale(0.2f, 0.2f, 0.2f);
	model_matrix = mat4::identity();
	model_matrix *= vmath::translation(-3.0f, 0.0f, -1.0f);
	glUniformMatrix4fv(modelview_matrix_loc, 1, GL_FALSE, model_matrix);
	Matrix4 pm, mm;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{ 
			pm[i * 4+j] = projection_matrix[i][j];
			mm[i * 4+j] = model_matrix[i][j];
		}
			
	}
	//float view[4];
	//glViewport(view);
	
	//Vector4 vv(824.0f, 568.0f,0.0f,1.0f);
	//vv = pm * vv;

	Vector4 vec(-1.f, 1.0f, 0.0f, 1.0);
	Vector4 vec2 = pm * (mm * vec);
	Vector3 v(vec2.x / vec2.w, vec2.y / vec2.w, vec2.z / vec2.w);


	widgetParam.rect.xmin = -1.0f;
	widgetParam.rect.xmax = 1.f;
	widgetParam.rect.ymin = -1.f;
	widgetParam.rect.ymax = 1.f;

	glUniform4fv(parameters_loc, MAX_WIDGET_PARAMETERS, (float*)&widgetParam);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL);
	glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, NULL, 1, 0, 0);
	//base::Display();
	if (mouseLeftDown)
	{
		bool bb = glIsEnabled(GL_SCISSOR_TEST);
		glEndQuery(GL_SAMPLES_PASSED);
		uint result = 0;
		glGetQueryObjectuiv(QueryIDs, GL_QUERY_RESULT, &result);
		bttt = result;
		glDeleteQueries(1, &result);
		mouseLeftDown = false;
			DrawRect();
	}
}
void DrawCommandExample::mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = false;
			_ttx = x;
			_tty = y;
		}
		else if (state == GLUT_UP)
		{
			mouseLeftDown = true;
			m_CrupRect.xmin = double(min(x, _ttx));
			m_CrupRect.xmax = double(max(x, _ttx));
			m_CrupRect.ymin = double(min(y, _tty));
			m_CrupRect.ymax = double(max(y, _tty));

		}
		
		
	}
	DrawRect();
	glutSwapBuffers();
}




/* ELEM#(v, ...): is the first arg equal any others? */
/* internal helpers. */
#define _VA_ELEM2(v, a) ((v) == (a))
#define _VA_ELEM3(v, a, b) \
  (_VA_ELEM2(v, a) || _VA_ELEM2(v, b))
#define _VA_ELEM4(v, a, b, c) \
  (_VA_ELEM3(v, a, b) || _VA_ELEM2(v, c))
#define _VA_ELEM5(v, a, b, c, d) \
  (_VA_ELEM4(v, a, b, c) || _VA_ELEM2(v, d))
#define _VA_ELEM6(v, a, b, c, d, e) \
  (_VA_ELEM5(v, a, b, c, d) || _VA_ELEM2(v, e))
#define _VA_ELEM7(v, a, b, c, d, e, f) \
  (_VA_ELEM6(v, a, b, c, d, e) || _VA_ELEM2(v, f))
#define _VA_ELEM8(v, a, b, c, d, e, f, g) \
  (_VA_ELEM7(v, a, b, c, d, e, f) || _VA_ELEM2(v, g))
#define _VA_ELEM9(v, a, b, c, d, e, f, g, h) \
  (_VA_ELEM8(v, a, b, c, d, e, f, g) || _VA_ELEM2(v, h))
#define _VA_ELEM10(v, a, b, c, d, e, f, g, h, i) \
  (_VA_ELEM9(v, a, b, c, d, e, f, g, h) || _VA_ELEM2(v, i))
#define _VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) \
  (_VA_ELEM10(v, a, b, c, d, e, f, g, h, i) || _VA_ELEM2(v, j))
#define _VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) \
  (_VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) || _VA_ELEM2(v, k))
#define _VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) \
  (_VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) || _VA_ELEM2(v, l))
#define _VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) \
  (_VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) || _VA_ELEM2(v, m))
#define _VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  (_VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) || _VA_ELEM2(v, n))
#define _VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
  (_VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) || _VA_ELEM2(v, o))
#define _VA_ELEM17(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
  (_VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) || _VA_ELEM2(v, p))
/* --- internal helpers --- */
#define _VA_NARGS_GLUE(x, y) x y
#define _VA_NARGS_RETURN_COUNT(\
  _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, \
  _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, \
  _33_, _34_, _35_, _36_, _37_, _38_, _39_, _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, \
  _49_, _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, _60_, _61_, _62_, _63_, _64_, \
  count, ...) count
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args
#define _VA_NARGS_OVERLOAD_MACRO2(name, count) name##count
#define _VA_NARGS_OVERLOAD_MACRO1(name, count) _VA_NARGS_OVERLOAD_MACRO2(name, count)
#define _VA_NARGS_OVERLOAD_MACRO(name,  count) _VA_NARGS_OVERLOAD_MACRO1(name, count)
/* --- expose for re-use --- */
/* 64 args max */
#define VA_NARGS_COUNT(...) _VA_NARGS_EXPAND((__VA_ARGS__, \
  64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
  48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
  32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
  16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2, 1, 0))
#define VA_NARGS_CALL_OVERLOAD(name, ...) \
  _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(name, VA_NARGS_COUNT(__VA_ARGS__)), (__VA_ARGS__))

#define ELEM(...) VA_NARGS_CALL_OVERLOAD(_VA_ELEM, __VA_ARGS__)


void DrawCommandExample::keyboardCB(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'b':
	{
		if (ELEM(1, 2, 3, 4))
			return;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawRect();
		//rctf cirrect;
		//cirrect.xmin = -0.8f;
		//cirrect.xmax = 0.8f;
		//cirrect.ymin = -0.8f;
		//cirrect.ymax = 0.8f;
		//DrawHSVCIRCLE(&cirrect);
		//base::Display();
		glutSwapBuffers();
	}
		break;
	case 'v':
	{
		bttt = true;
		//剪切测试
		DrawRect();
		glutSwapBuffers();
	}
	break;
	}
}


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

#pragma region DrawHSVCIRCLE


void DrawCommandExample::DrawHSVCIRCLE(const rctf* rect)
{
    const int tot = 64;
    GPUVertFormat* format = immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
	uint color = GPU_vertformat_attr_add(format, "color", GPU_COMP_F32, 3);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	//glEnableVertexAttribArray(1);
	immBindProgram(GPU_SHADER_2D_SMOOTH_COLOR);
	

	//glEnable(GL_DEPTH_TEST);
	immBegin(GL_TRIANGLE_FAN, tot + 2);
	float/* rgb[3],*/ hsv[3], rgb_center[3];
    const float centx = 0.0f;
    const float centy = 0.0f;
	hsv[0] = 0.08f; hsv[1] = 1.0f; hsv[2] = 1.0f;
	rgb_center[0] = 1; rgb_center[1] = 1; rgb_center[2] = 1;

	immAttr3f(color, rgb_center[0], rgb_center[1],rgb_center[2]);
	immVertex2f(pos, centx, centy);
    
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

		immAttr3f(color, rgb_ang[0],rgb_ang[1],rgb_ang[2]);
		immVertex2f(pos, centx + co * radius, centy + si * radius);
	}
	
	immEnd();
	
	glUseProgram(0);
	{
		GPUVertFormat* format = immVertexFormat();
		uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
		//glDisableVertexAttribArray(1);
		immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		immUniformColor3f(0.1f, 0.1f, 0.2f);
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

uint DrawCommandExample::GPU_vertformat_attr_add(GPUVertFormat* format, const char* name, GPUVertCompType comp_type, uint comp_len)
{
	format->name_len++; /* multiname support */

	const uint attr_id = format->attr_len++;
	GPUVertAttr* attr = &format->attrs[attr_id];

	//attr->names[attr->name_len++] = copy_attr_name(format, name);
	attr->comp_type = comp_type;
	attr->gl_comp_type = convert_comp_type_to_gl(comp_type);
	attr->comp_len = (comp_type == GPU_COMP_I10) ?4 :comp_len; /* system needs 10_10_10_2 to be 4 or BGRA */
	attr->sz = attr_sz(attr);
	attr->offset = 0; /* offsets & stride are calculated later (during pack) */
	//attr->fetch_mode = fetch_mode;
    return attr_id;
}

uint DrawCommandExample::attr_sz(const GPUVertAttr* a)
{
	if (a->comp_type == GPU_COMP_I10) {
		return 4; /* always packed as 10_10_10_2 */
	}
	const GLubyte sizes[] = { 1, 1, 2, 2, 4, 4, 4 };
	uint usize = sizes[a->comp_type];

	return a->comp_len * usize;
}

GLenum DrawCommandExample::convert_comp_type_to_gl(GPUVertCompType type)
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

void DrawCommandExample::GPU_vertformat_clear(GPUVertFormat* format)
{
	memset(format, 0, sizeof(GPUVertFormat));
}

GPUVertFormat* DrawCommandExample::immVertexFormat()
{
	GPU_vertformat_clear(&imm.vertex_format);
	return &imm.vertex_format;
}

uint vertex_buffer_size(const GPUVertFormat* format, uint vertex_len)
{
	return format->stride * vertex_len;
}

static uint padding(uint offset, uint alignment)
{
	const uint mod = offset % alignment;
	return (mod == 0) ? 0 : (alignment - mod);
}

void DrawCommandExample::immBegin(GLuint prim_type, uint vertex_len)
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

void DrawCommandExample::immEnd()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
	if (imm.vertex_len > 0) {
		immDrawSetup();
		glDrawArrays(imm.prim_type, 0, imm.vertex_len);
		buffer_offset += imm.buffer_bytes_mapped;
	}
}

void DrawCommandExample::immBindProgram(GLuint program)
{
	glUseProgram(program);
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 400.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_FALSE, projection_matrix);
	//// Draw Arrays...
	mat4 model_matrix = vmath::translation(0.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, model_matrix);

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
	imm.shader_interface = program;
}

void DrawCommandExample::immDrawSetup(void)
{
	/* set up VAO -- can be done during Begin or End really */
	glBindVertexArray(imm.vao_id);

	if(imm.vertex_format.attr_len > 1)
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

void DrawCommandExample::immEndVertex(void)
{
    //imm.vertex_idx++;
	imm.vertex_data += imm.vertex_format.stride;
}

void DrawCommandExample::immAttr3f(uint attr_id, float x, float y, float z)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];
	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
	data[2] = z;
}

void DrawCommandExample::immVertex2f(uint attr_id, float x, float y)
{
	immAttr2f(attr_id, x, y);
	immEndVertex();
}

void DrawCommandExample::immAttr2f(uint attr_id, float x, float y)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];

	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
}

float DrawCommandExample::len_squared_v2(const float v[2])
{
	return v[0] * v[0] + v[1] * v[1];
}

void DrawCommandExample::ui_hsvcircle_vals_from_pos(const rctf* rect, const float mx, const float my, float* r_val_rad, float* r_val_dist)
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

void DrawCommandExample::hsv_to_rgb_v(const float hsv[3], float r_rgb[3])
{
	hsv_to_rgb(hsv[0], hsv[1], hsv[2], &r_rgb[0], &r_rgb[1], &r_rgb[2]);
}

#define CLAMP(a, b, c) \
  { \
    if ((a) < (b)) \
      (a) = (b); \
    else if ((a) > (c)) \
      (a) = (c); \
  } 
void DrawCommandExample::hsv_to_rgb(float h, float s, float v, float* r_r, float* r_g, float* r_b)
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

void DrawCommandExample::ui_hsv_cursor(float x, float y)
{
	GPUVertFormat* format = immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
	immBindProgram(GPU_SHADER_2D_UNIFORM_COLOR);
	immUniformColor3f(1, 1, 1);
	imm_draw_circle(GL_TRIANGLE_FAN, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 8);

	//immUnbindProgram();
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	immUniformColor3f(0, 0, 0);
	//imm_draw_circle(GL_TRIANGLE_FAN, pos, x,y, 0.03f, 0.03f, 8);
	imm_draw_circle(GL_LINE_LOOP, pos, x, y, 3 * U.pixelsize, 3 * U.pixelsize, 12);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

void DrawCommandExample::immUniformColor3f(float r, float g, float b)
{
	GLuint uniform_loc = glGetUniformLocation(imm.shader_interface, "color");
	const float scale = 1.0f;// / 255.0f;`
	glUniform4f(uniform_loc, r * scale, g * scale, b * scale, 1.0);
}

void DrawCommandExample::imm_draw_circle(GLuint prim_type,const uint shdr_pos,float x,float y,float rad_x,float rad_y,int nsegments)
{
	immBegin(prim_type, nsegments);
	for (int i = 0; i < nsegments; i++) {
		const float angle = (float)(2 * M_PI) * ((float)i / (float)nsegments);
		immVertex2f(shdr_pos, x + (rad_x * cosf(angle)), y + (rad_y * sinf(angle)));
	}
	immEnd();
}

#pragma endregion DrawHSVCIRCLE