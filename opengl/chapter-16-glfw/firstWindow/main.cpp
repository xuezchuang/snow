
#if defined(_MSC_VER)
// Make MS math.h define M_PI
#define _USE_MATH_DEFINES
#endif
//#include "D:\Aries\git\glfw\deps\glad/gl.h"
//#include <GL/glew.h>
#include "glad/gl.h"
#include "GLFW/glfw3.h"



//#include "D:\Aries\git\glfw\include\GLFW/glfw3.h"


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow* window);

int main()
{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
		glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

		GLFWwindow *window = glfwCreateWindow(640,480,"learn opengl",NULL,NULL);
		if(window == NULL)
		{
				std::cout<<"failed create window"<<std::endl;
				glfwTerminate();
				return -1;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

		if (!gladLoadGL(glfwGetProcAddress))
		{
			std::cout << "glfw load gl loader failed." << std::endl;
			return -1;
		}

		while(!glfwWindowShouldClose(window))
		{
				processInput(window);
				glClearColor(0.2f,0.3f,0.3f,1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				glfwSwapBuffers(window);
				glfwPollEvents();

		}

		glfwTerminate();
		return 0;
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
		glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window)
{
		if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
				glfwSetWindowShouldClose(window,GL_TRUE);
		}
}
