#include <iostream>

class Controller
{	
	public:
		Controller(){}
		virtual void begin(){}
		virtual void update(){}
		
		int mouseX2;
		int mouseY2;
		virtual void startmouse(int _x, int _y)
		{
			mouseX2 = _x;
			mouseY2 = _y;
		}
		virtual void mouse(int _x, int _y) {}
		virtual void keyboard(int, int){std::cout << "No keyboard handler" << std::endl;}
};
