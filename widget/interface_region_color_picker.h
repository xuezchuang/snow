#pragma once
#include "interface_intern.h"

/* Sensor fit */
class IRColorPicker
{
public:
	IRColorPicker();
	~IRColorPicker();
	void DrawHSVCIRCLE(const rctf* rect);
private:
	void immUniformColor3f(float r, float g, float b);
	void ui_hsv_cursor(float x, float y);
};