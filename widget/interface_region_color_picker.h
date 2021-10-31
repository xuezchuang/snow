#pragma once
#include "interface_widgets.h"
class IRColorPicker : public INWidget
{
public:
	IRColorPicker();
	~IRColorPicker();
	virtual void draw();
	//void set_radio(float radio);
	bool update(float x, float y);
	void getcolor(float rgbcolor[4]);
private:
	void immUniformColor3f(float r, float g, float b);
	void ui_hsv_cursor(float x, float y);
	void imm_draw_circle(GLuint prim_type, const uint shdr_pos, float x, float y, float rad_x, float rad_y, int nsegments);
	//void ui_hsvcircle_vals_from_pos(const float mx, const float my, float* r_val_rad, float* r_val_dist);
private:
	//Բ�뾶 pixel unit
	float m_radio = 100.0f;
	float m_hsv[3];
};