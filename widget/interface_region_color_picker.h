#pragma once
#include "interface_intern.h"
#include "interface_widgets.h"
class IRColorPicker : public INWidget
{
public:
	IRColorPicker();
	~IRColorPicker();
	virtual void draw(const rctf* rect);
	void set_radio(float radio);
private:
	void immUniformColor3f(float r, float g, float b);
	void ui_hsv_cursor(float x, float y);
	void imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments);

private:
	//Բ�뾶 pixel unit
	float m_radio = 100.0f;
};