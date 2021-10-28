#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "BLI_camera.h"
#include "interface_region_color_picker.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouseButton_callback(GLFWwindow* window, int button, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
void wm_draw_update();
// 屏幕宽高
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

float factor = 0.2f;
float fov = 45.0f;

//摄像机
BLICamera m_camera;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 摄像机
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 光照
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

#include "GPUShader.h"
#include "interface_intern.h"

bool bredraw = true;
GLint modelview_matrix_loc;
GLint project_matrix_loc;
GLint parameters_loc;
GLint test_loc;
GLint vcolor_loc;
GLuint vao[1];
GLuint vbo[1];
GLuint ebo[1];
GLuint vaoTest;
GLuint vboTest;

#define USE_PRIMITIVE_RESTART 1


typedef unsigned int				uint;


UserDef U;
GLFWwindow* window;

float aspect;
void _InitVAO()
{
	m_camera.setortho(false);
	m_camera.BKE_camera_params_init();
	m_camera.BKE_camera_params_from_view3d();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	aspect = float(viewport[3]) / viewport[2];
	IGPUShader::Instance()->bindShader(render_prog);
	// "model_matrix" is actually an array of 4 matrices
	modelview_matrix_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(render_prog)->program, "model_matrix");
	project_matrix_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(render_prog)->program, "projection_matrix");
	parameters_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(render_prog)->program, "parameters");
	vcolor_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(render_prog)->program, "vcolor");
	test_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(render_prog)->program, "btest");
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
	//Test
	{
		float vertices[] = {
			// 位置              // 法线            // 纹理坐标
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
		};
		glGenVertexArrays(1, &vaoTest);
		glBindVertexArray(vaoTest);
		glGenBuffers(1, &vboTest);
		glBindBuffer(GL_ARRAY_BUFFER, vboTest);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 位置属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	}
	
	{
		IGPUShader::Instance()->bindShader(GPU_SHADER_2D_SMOOTH_COLOR);

		//GLuint pos = glGetAttribLocation(GPU_SHADER_2D_UNIFORM_COLOR, "position");
		GLuint pos = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "pos");
		GLuint color = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "color");

		//glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, NULL);
		//glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vertex_positions));
		glEnableVertexAttribArray(pos);
		glEnableVertexAttribArray(color);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	//glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	{
		U.pixelsize = 0.01f;
	}
}
#include "LoadShaders.h"
#include "vmath.h"
using namespace vmath;
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Widget", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouseButton_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewInit();
	_InitVAO();

	//主循环
	while (!glfwWindowShouldClose(window)) {
		wm_draw_update();
	}

	glfwTerminate();

	return 0;
}

void DrawRect();
void DrawTest();

/* keep in sync with shader */
#define MAX_WIDGET_BASE_BATCH 6
#define MAX_WIDGET_PARAMETERS 12


BLI_INLINE float BLI_rctf_size_x(const struct rctf* rct)
{
	return (rct->xmax - rct->xmin);
}
BLI_INLINE float BLI_rctf_size_y(const struct rctf* rct)
{
	return (rct->ymax - rct->ymin);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(FORWARD, deltaTime);
		bredraw = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(BACKWARD, deltaTime);
		bredraw = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(LEFT, deltaTime);
		bredraw = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(RIGHT, deltaTime);
		bredraw = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(UP, deltaTime);
		bredraw = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_camera.ProcessKeyboard(DOWN, deltaTime);
		bredraw = true;
	}*/
}
double xpos, ypos;
bool mouseLeftDown = false;
rctf m_CrupRect;
float mval[2];
bool bmoveing = false;
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (bmoveing)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mval[0] = float(x - xpos);
		mval[1] = float(ypos - y);
		xpos = x;
		ypos = y;
		bredraw = true;
	}
}


void mouseButton_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (action == 1)
		{
			glfwGetCursorPos(window, &xpos, &ypos);
			mouseLeftDown = false;
		}
		else if (action == 0)
		{
			mouseLeftDown = true;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			m_CrupRect.xmin = float(min(x, xpos));
			m_CrupRect.xmax = float(max(x, xpos));
			m_CrupRect.ymin = float(min(y, ypos));
			m_CrupRect.ymax = float(max(y, ypos));
			bredraw = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_3)
	{
		if (action == 1)
		{
			glfwGetCursorPos(window, &xpos, &ypos);
			bmoveing = true;
			m_camera.ED_view3d_calc_zfac(nullptr);
		}
		else if (action == 0)
		{
			bmoveing = false;
			//mouseLeftDown = true;
			//double x, y;
			//glfwGetCursorPos(window, &x, &y);
			//m_CrupRect.xmin = float(min(x, xpos));
			//m_CrupRect.xmax = float(max(x, xpos));
			//m_CrupRect.ymin = float(min(y, ypos));
			//m_CrupRect.ymax = float(max(y, ypos));
			//bredraw = true;
		}
	}
}

//鼠标滚轮消息回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	m_camera.ProcessMouseScroll(float(yoffset));
	bredraw = true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	bredraw = true;
}

//加载纹理
unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "纹理加载失败，路径是:" << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



void wm_draw_update()
{
	float currentFrame = float(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	processInput(window);
	if (bredraw)
	{
		bredraw = false;
		//DrawRect();
		DrawTest();

		IRColorPicker Selectcolor[2];
		Selectcolor[0].SetOff(120.0f, 120.0f);
		Selectcolor[1].SetOff(450.0f, 120.0f);
		rctf cirrect;
		cirrect.xmin = -1.0f;
		cirrect.xmax = 1.0f;
		cirrect.ymin = -1.0f;
		cirrect.ymax = 1.0f;
		Selectcolor[0].draw(&cirrect);
		Selectcolor[1].draw(&cirrect);
		glfwSwapBuffers(window);
	}
	glfwPollEvents();
}


bool bttt = false;


uiWidgetBaseParameters widgetParam;



void DrawRect()
{
	//glm::mat4 model_matrix;
	mat4 model_matrix;

	//glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Activate simple shading program
	//glUseProgram(render_prog);
	IGPUShader::Instance()->bindShader(render_prog);
	// Set up the model and projection matrix
	//vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 400.0));
	rctf viewplane;
	viewplane.xmin = -1.0f;		viewplane.xmax = 1.0f;
	viewplane.ymin = -aspect;	viewplane.ymax = aspect;

	
	model_matrix = mat4::identity();
	model_matrix *= vmath::translation(0.0f, 0.0f, -0.1f);
	//model_matrix *= vmath::scale(0.8f, 0.8f, 1.f);
	float dscale = 5.0;
	{
		viewplane.xmin *= dscale;
		viewplane.xmax *= dscale;
		viewplane.ymin *= dscale;
		viewplane.ymax *= dscale;
	}
	vmath::mat4 projection_matrix(vmath::frustum(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, 0.1f, 100.0f));
	//projection_matrix = vmath::perspective(1, aspect, 0.1f, 500.0f);
	if (1)
	{
		//projection_matrix = vmath::Orthogonal(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, -500.0f, 500.0f);
	}
	else
	{
		//model_matrix *= vmath::scale(0.2f, 0.2f, 0.1f);
	}


	GLuint QueryIDs;
	glUniform4fv(vcolor_loc, 1, vec4(1.0f, 0.0f, 0.0f, 1.0f));
	GLfloat bb = 2;
	if (bttt)
		glUniform4fv(vcolor_loc, 1, vec4(0.0f, 0.0f, 1.0f, 1.0f));
	if (mouseLeftDown)
	{
		glEnable(GL_SCISSOR_TEST);
		glGenQueries(1, &QueryIDs);
		glBeginQuery(GL_SAMPLES_PASSED, QueryIDs);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		m_CrupRect.xmin = viewplane.xmin + (BLI_rctf_size_x(&viewplane) * (m_CrupRect.xmin / viewport[2]));
		m_CrupRect.ymin = viewplane.ymin + (BLI_rctf_size_y(&viewplane) * (m_CrupRect.ymin / viewport[3]));
		m_CrupRect.xmax = viewplane.xmin + (BLI_rctf_size_x(&viewplane) * (m_CrupRect.xmax / viewport[2]));
		m_CrupRect.ymax = viewplane.ymin + (BLI_rctf_size_y(&viewplane) * (m_CrupRect.ymax / viewport[3]));
		
		projection_matrix = vmath::Orthogonal(m_CrupRect.xmin, m_CrupRect.xmax, m_CrupRect.ymin, m_CrupRect.ymax, -400.0f, 400.0f);
		//计算出projection_matrix 后计算6个面对所有构件CPU剔除(简单快速)后,进行使用GPU裁剪
	}

	glUniformMatrix4fv(project_matrix_loc, 1, GL_FALSE, projection_matrix);
	// Set up for a glDrawElements call
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	//// Draw Arrays...
	
	//glUniformMatrix4fv(modelview_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(modelview_matrix_loc, 1, GL_FALSE, model_matrix);

	widgetParam.rect.xmin = -1.0f;
	widgetParam.rect.xmax = 1.f;
	widgetParam.rect.ymin = -1.f;
	widgetParam.rect.ymax = 1.f;

	glUniform4fv(parameters_loc, MAX_WIDGET_PARAMETERS, (float*)&widgetParam);
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
void DrawTest()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(GPU_SHADER_3D_TEST);
	
	IGPUShader::Instance()->bindShader(GPU_SHADER_3D_TEST);
	//glEnable(GL_DEPTH_TEST);
	//tt.setortho(false);
	//tt.update();
	float viewport_size[4];
	glGetFloatv(GL_VIEWPORT, viewport_size);
	rctf viewplane;


	m_camera.update();
	viewplane = m_camera.getViewplane();
	vmath::mat4 projection_matrix;
	GLuint QueryIDs;
	if (mouseLeftDown)
	{
		glEnable(GL_SCISSOR_TEST);
		glGenQueries(1, &QueryIDs);
		glBeginQuery(GL_SAMPLES_PASSED, QueryIDs);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		rctf temp = viewplane;

		viewplane.xmin = temp.xmin + (BLI_rctf_size_x(&temp) * (m_CrupRect.xmin / viewport[2]));
		viewplane.ymin = temp.ymin + (BLI_rctf_size_y(&temp) * (m_CrupRect.ymin / viewport[3]));
		viewplane.xmax = temp.xmin + (BLI_rctf_size_x(&temp) * (m_CrupRect.xmax / viewport[2]));
		viewplane.ymax = temp.ymin + (BLI_rctf_size_y(&temp) * (m_CrupRect.ymax / viewport[3]));

		glViewport(0, 0, GLsizei(m_CrupRect.xmax - m_CrupRect.xmin), GLsizei(m_CrupRect.ymax - m_CrupRect.ymin));
		//projection_matrix = vmath::Orthogonal(m_CrupRect.xmin, m_CrupRect.xmax, m_CrupRect.ymin, m_CrupRect.ymax, -400.0f, 400.0f);
		//计算出projection_matrix 后计算6个面对所有构件CPU剔除(简单快速)后,进行使用GPU裁剪
	}

	
	if (0)
		projection_matrix = vmath::Orthogonal(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, -500, 500.0f);
	else
		projection_matrix = vmath::frustum(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, 0.01f, 1000.0f);
	//projection_matrix = vmath::Orthogonal(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, -500.0f, 500.0f);


	vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
	if (bttt)
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	GLuint Test_project_matrix_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_3D_TEST)->program, "projection_matrix");
	glUniformMatrix4fv(Test_project_matrix_loc, 1, GL_FALSE, projection_matrix);
	// Set up for a glDrawElements call
	glBindVertexArray(vaoTest);
	glBindBuffer(GL_ARRAY_BUFFER, vboTest);
	//mat4 model_matrix = mat4::identity();
	//model_matrix *= vmath::translation(0.0f, 0.0f, -.1f);
	//model_matrix *= vmath::scale(0.3f, 0.3f, 0.3f);
	//glm::mat4 model_matrix = m_camera.GetViewMatrix();
	float model_matrix[4][4];
	
	m_camera.ED_view3d_win_to_delta(mval);
	m_camera.ED_view3d_update_viewmat();
	m_camera.getviewmatrix(model_matrix);
	GLuint Test_model_matrix_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_3D_TEST)->program, "model_matrix");
	glUniformMatrix4fv(Test_model_matrix_loc, 1, GL_FALSE, model_matrix[0]);
	//glUniformMatrix4fv(Test_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	GLuint Test_color_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_3D_TEST)->program, "color");
	glUniform4fv(Test_color_loc, 1, color);

	glDisableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	if (mouseLeftDown)
	{
		bool bb = glIsEnabled(GL_SCISSOR_TEST);
		glEndQuery(GL_SAMPLES_PASSED);
		uint result = 0;
		glGetQueryObjectuiv(QueryIDs, GL_QUERY_RESULT, &result);
		bttt = result;
		glDeleteQueries(1, &result);
		mouseLeftDown = false;
		DrawTest();
	}
}


