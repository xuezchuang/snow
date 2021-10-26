#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "BLI_camera.h"

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
#include "GPU_format.h"
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




typedef struct {
	GLuint shader_interface;
	uint vertex_len;
	uint buffer_bytes_mapped;
	GLuint vao_id;
	GLuint prim_type;
	GPUVertFormat vertex_format;
	GLubyte* vertex_data;
} Immediate;
Immediate imm;
GLuint Imm_buffer_vbo_id;
uint buffer_size;
uint buffer_offset;
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
	//
	{
		IGPUShader::Instance()->bindShader(GPU_SHADER_2D_SMOOTH_COLOR);
		// Set up the vertex attributes
		glGenVertexArrays(1, &imm.vao_id);
		glBindVertexArray(imm.vao_id);

		glGenBuffers(1, &Imm_buffer_vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, Imm_buffer_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, 4 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);


		//GLuint pos = glGetAttribLocation(GPU_SHADER_2D_UNIFORM_COLOR, "position");
		GLuint pos = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "pos");
		GLuint color = glGetAttribLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_2D_SMOOTH_COLOR)->program, "color");

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
#include "LoadShaders.h"
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
	//glfwSetCursorPosCallback(window, mouse_callback);
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
void DrawHSVCIRCLE(const rctf* rect);


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

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	//if (firstMouse) {
	//	lastX = xPos;
	//	lastY = yPos;
	//	firstMouse = false;
	//}

	//float xoffset = lastX - xPos;	//别忘了，在窗口中，左边的坐标小于右边的坐标，而我们需要一个正的角度
	//float yoffset = lastY - yPos;	//同样，在窗口中，下面的坐标大于上面的坐标，而我们往上抬头的时候需要一个正的角度
	//lastX = xPos;
	//lastY = yPos;

	//camera.ProcessMouseMovement(xoffset, yoffset);

}
double xpos, ypos;
bool mouseLeftDown = false;
rctf m_CrupRect;
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
		float viewport_size[4];
		glGetFloatv(GL_VIEWPORT, viewport_size);
		rctf cirrect;
		cirrect.xmin = -1.0f;
		cirrect.xmax = 1.0f;
		cirrect.ymin = -1.0f;
		cirrect.ymax = 1.0f;
		DrawHSVCIRCLE(&cirrect);
		glfwSwapBuffers(window);
	}
	glfwPollEvents();
}

#include "Matrices.h"
#include "vmath.h"
bool bttt = false;


uiWidgetBaseParameters widgetParam;

/* keep in sync with shader */
#define MAX_WIDGET_BASE_BATCH 6
#define MAX_WIDGET_PARAMETERS 12
using namespace vmath;
BLI_INLINE float BLI_rctf_size_x(const struct rctf* rct)
{
	return (rct->xmax - rct->xmin);
}
BLI_INLINE float BLI_rctf_size_y(const struct rctf* rct)
{
	return (rct->ymax - rct->ymin);
}

void DrawRect()
{
	//glm::mat4 model_matrix;
	mat4 model_matrix;

	//glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
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
	Vector4 vec2 = pm * vec;
	//Vector3 v(vec2.x / vec2.w, vec2.y / vec2.w, vec2.z / vec2.w);
	Vector4 tempv(vec2.x, vec2.y, vec2.z, 0.0);
	Vector4 temp = tempv * pm.invert();


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
	m_camera.ED_view3d_update_viewmat();
	m_camera.getviewmatrix(model_matrix);
	GLuint Test_model_matrix_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_3D_TEST)->program, "model_matrix");
	glUniformMatrix4fv(Test_model_matrix_loc, 1, GL_FALSE, model_matrix[0]);
	//glUniformMatrix4fv(Test_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	GLuint Test_color_loc = glGetUniformLocation(IGPUShader::Instance()->GetShader(GPU_SHADER_3D_TEST)->program, "color");
	glUniform4fv(Test_color_loc, 1, color);

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


void GPU_vertformat_clear(GPUVertFormat* format)
{
	memset(format, 0, sizeof(GPUVertFormat));
}

GPUVertFormat* immVertexFormat()
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

void immBegin(GLuint prim_type, uint vertex_len)
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

void immBindProgram(eGPUBuiltinShader program)
{
	IGPUShader::Instance()->bindShader(program);

	vmath::mat4 projection_matrix(vmath::Orthogonal(0.0f, float(SCR_WIDTH), 0.0f, float(SCR_HEIGHT), -500.0f, 500.0f));
	glUniformMatrix4fv(glGetUniformLocation(IGPUShader::Instance()->GetShader(program)->program, "projection_matrix"), 1, GL_FALSE, projection_matrix);
	mat4 model_matrix = vmath::translation(120.0f, SCR_HEIGHT-120.0f, 0.0f);
	model_matrix *= vmath::scale(100.0f);
	glUniformMatrix4fv(glGetUniformLocation(IGPUShader::Instance()->GetShader(program)->program, "model_matrix"), 1, GL_FALSE, model_matrix);

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

void immDrawSetup(void)
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

void immEnd()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	if (imm.vertex_len > 0) {
		immDrawSetup();
		glDrawArrays(imm.prim_type, 0, imm.vertex_len);
		buffer_offset += imm.buffer_bytes_mapped;
	}
}
void immEndVertex(void)
{
	//imm.vertex_idx++;
	imm.vertex_data += imm.vertex_format.stride;
}

void immAttr3f(uint attr_id, float x, float y, float z)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];
	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
	data[2] = z;
}

void immAttr2f(uint attr_id, float x, float y)
{
	GPUVertAttr* attr = &imm.vertex_format.attrs[attr_id];

	float* data = (float*)(imm.vertex_data + attr->offset);

	data[0] = x;
	data[1] = y;
}

void immVertex2f(uint attr_id, float x, float y)
{
	immAttr2f(attr_id, x, y);
	immEndVertex();
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


void immUniformColor3f(float r, float g, float b)
{
	GLuint uniform_loc = glGetUniformLocation(imm.shader_interface, "color");
	const float scale = 1.0f;// / 255.0f;`
	glUniform4f(uniform_loc, r * scale, g * scale, b * scale, 1.0);
}

void imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments)
{
	immBegin(prim_type, nsegments);
	for (int i = 0; i < nsegments; i++) {
		const float angle = (float)(2 * M_PI) * ((float)i / (float)nsegments);
		immVertex2f(shdr_pos, x + (rad_x * cosf(angle)), y + (rad_y * sinf(angle)));
	}
	immEnd();
}

void ui_hsv_cursor(float x, float y)
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

void DrawHSVCIRCLE(const rctf* rect)
{
	const int tot = 64;
	GPUVertFormat* format = immVertexFormat();
	uint pos = GPU_vertformat_attr_add(format, "pos", GPU_COMP_F32, 2);
	uint color = GPU_vertformat_attr_add(format, "color", GPU_COMP_F32, 3);
	immBindProgram(GPU_SHADER_2D_SMOOTH_COLOR);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	
	immBegin(GL_TRIANGLE_FAN, tot + 2);
	float/* rgb[3],*/ hsv[3], rgb_center[3];
	const float centx = 0.0;
	const float centy = 0.0;
	//hsv[0] = 0.08f; hsv[1] = 1.0f; hsv[2] = 1.0f;
	hsv[0] = 0.0163f; hsv[1] = 0.9875f; hsv[2] = 1.0f;
	rgb_center[0] = 1; rgb_center[1] = 1; rgb_center[2] = 1;

	immAttr3f(color, rgb_center[0], rgb_center[1], rgb_center[2]);
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

		immAttr3f(color, rgb_ang[0], rgb_ang[1], rgb_ang[2]);
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

#pragma endregion 