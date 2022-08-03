#include "Game.hpp"
#include "Dimension.hpp"
#include <iostream>

void Game::init(GLFWwindow* w, float pw, float ph)
{
	window = w;
	_matrix.init(pw, ph);
	Dimension::set(pw, ph);
}

void Game::update()
{
	if(newController==NULL)
	{
		controller->update();
		glfwPollEvents();
	}
	else
	{
		delete controller;
		controller = newController;
		newController = nullptr;
		controller->begin();
		controller->update();
		glfwPollEvents();
	}
}

void Game::changeController(Controller* c)
{
	newController = c;
}

void Game::flip()
{
	glfwSwapBuffers(window);
}
double x, y;
bool bfirst = false;
void Game::mouseClickCallback(GLFWwindow* w, int b, int a, int modifiers)
{
	bfirst = !bfirst;
	if (bfirst)
	{
		glfwGetCursorPos(w, &x, &y);

		std::cout << "M: " << x << " " << y << std::endl;

		float fx = 2 * (x - Dimension::getWidth() / 2) / Dimension::getHeight();
		float fy = 2 * (Dimension::getHeight() / 2 - y) / Dimension::getHeight();

		controller->startmouse(x, y);
	}

	//controller->mouse(b, a);//, fx, fy);
}

void Game::mousemove(GLFWwindow* w, double xpos, double ypos)
{
	if (bfirst)
	{
		controller->mouse((int)xpos, (int)ypos);
	}
}

void Game::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	controller->keyboard(key, action);
}

GLFWwindow* Game::window = NULL;
Matrix Game::_matrix;
Controller* Game::controller = NULL;
Controller* Game::newController = NULL;
