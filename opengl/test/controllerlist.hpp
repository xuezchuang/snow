#ifndef CONTROLLERLIST_H
#define CONTROLLERLIST_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <time.h>
#include <vector>

#include "Controller.h"


class SceneController:	public Controller
{
  GLuint vao;
  GLuint vbo[2];
  GLuint ubo;
  
  int fCount = 0;
	
	public:
		
		virtual void keyboard(int, int);
		
		void update();
		SceneController();
				

		
};

#endif
