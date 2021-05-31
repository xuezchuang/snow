#include <iostream>

class Controller
{	
	public:
		Controller(){}
		virtual void begin(){}
		virtual void update(){}
		
		int x;
		int y;
		virtual void startmouse(int _x, int _y)
		{
			x = _x;
			y = _y;
		}
		virtual void mouse(int _x, int _y) {}
		virtual void keyboard(int, int){std::cout << "No keyboard handler" << std::endl;}
};
