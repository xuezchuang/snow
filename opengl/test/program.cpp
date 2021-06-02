#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <iostream>
#include "controllerlist.hpp"
#include "Game.hpp"
#include <cstring>
#include "Dimension.hpp"
#include "Shader/SwitchShader.hpp"
#include "helpful.hpp"

void processInput(GLFWwindow* window);


int main(int args, char* argv[])
{
    glfwInit();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = 400;
    int height = 400;
    GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);

    glfwMakeContextCurrent(window);



    glewExperimental=GL_TRUE;
    glewInit();

    std::cout << "GL context created" << std::endl;

    checkGlError("Context error");
    glfwSetKeyCallback(window, Game::keyCallback);
    glfwSetMouseButtonCallback(window, Game::mouseClickCallback);
    glfwSetCursorPosCallback(window, Game::mousemove);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_CLAMP);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderManager::init();
    Game::init(window, width, height);

    glViewport(0, 0, Dimension::getWidth(), Dimension::getHeight());


    Game::changeController(new SceneController());


    const GLubyte* s = glGetString(GL_VERSION);

    std::cout << "Version: " << s << std::endl;


    glfwPollEvents();


    //cout << "Starting loop" << std::endl;;
    while (true)
    {
        //glfwSwapBuffers(window);
        Game::update();
    }

}

void processInput(GLFWwindow* window)
{
    int key = 0;
    int scancode = 0;
    int action = 0;
    int mods = 0;
    Game::keyCallback(window, key, scancode, action, mods);
}