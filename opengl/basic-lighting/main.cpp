#include "Shader.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "stb_image.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// ��Ļ���
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float factor = 0.2f;
float fov = 45.0f;

//�����
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// �����
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ����
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float vertices[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

//
void getVertexAttribList(GLint programHandle)
{
	GLint numAttribs;
	glGetProgramInterfaceiv(programHandle, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &numAttribs);
	//glget
	GLenum properties[]{ GL_NAME_LENGTH,GL_TYPE,GL_LOCATION };
	//cout << "Active attributes:" << endl;
	for (int i = 0; i < numAttribs; ++i)
	{
		GLint results[3];
		glGetProgramResourceiv(programHandle, GL_PROGRAM_OUTPUT, i, 3, properties, 3, nullptr, results);
		
		GLint nameBufSize = results[0] + 1;
		GLchar* name = new char[nameBufSize];
		glGetProgramResourceName(programHandle, GL_PROGRAM_OUTPUT, i, nameBufSize, nullptr, name);

		GLint nindex = glGetProgramResourceIndex(programHandle, GL_PROGRAM_OUTPUT, name);
		//glm::vec4* abc =  (glm::vec4*)glGetActiveAttrib(programHandle, nindex, NULL, &a, &b, &c, name);

		//cout << "location:" << results[2] << "    " << name << "(" << getTypeString(results[1]) << ")" << endl;
		delete[] name;
	}
}




int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	GLenum glewStatus = glewInit();
	//if (!gladLoadGL(glfwGetProcAddress)) {
	//	std::cout << "Failed to initialize GLAD" << std::endl;
	//	return -1;
	//}

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	//��������ɫ��
	Shader lightingShader("Shader.vs", "Shader.fs");
	Shader lampShader("Shader.vs", "LampShader.fs");

	// VBO������VAO
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ����������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// ��ԴVAO (VBO��ͬ����Ϊ����������ͬһ��)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// λ�����ԣ�ֻ��Ҫ���¿�ȾͿ����ˣ�
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	////test
	//unsigned int testVao;
	//glGenVertexArrays(1, &testVao);
	//glBindVertexArray(testVao);

	//int matrix_loc = glGetAttribLocation(lightingShader.ID, "model_matrix");
	//glBindBuffer(GL_ARRAY_BUFFER, testVao);
	//glVertexAttribPointer(matrix_loc, 1, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	//glEnableVertexAttribArray(matrix_loc);

	//��ѭ��
	while (!glfwWindowShouldClose(window)) {
		//ÿһ֡��ʱ���߼�
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		//ͶӰ
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightingShader.setMat4("projection", glm::value_ptr(projection));

		//�۲�
		glm::mat4 view;
		view = camera.GetViewMatrix();
		lightingShader.setMat4("view", glm::value_ptr(view));

		//ģ��1
		glm::mat4 model;
		lightingShader.setMat4("model", glm::value_ptr(model));
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		////
		//glBindBuffer(GL_ARRAY_BUFFER, testVao);
		//glm::mat4* matrices = (glm::mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		getVertexAttribList(lightingShader.ID);
		//ģ��2
		lampShader.use();
		lampShader.setMat4("projection", glm::value_ptr(projection));
		lampShader.setMat4("view", glm::value_ptr(view));
		
		glm::mat4 model2;
		model2 = glm::translate(model2, lightPos);
		model2 = glm::scale(model2, glm::vec3(0.2f));
		lampShader.setMat4("model", glm::value_ptr(model2));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xoffset = lastX - xPos;	//�����ˣ��ڴ����У���ߵ�����С���ұߵ����꣬��������Ҫһ�����ĽǶ�
	float yoffset = lastY - yPos;	//ͬ�����ڴ����У���������������������꣬����������̧ͷ��ʱ����Ҫһ�����ĽǶ�
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

//��������Ϣ�ص�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
