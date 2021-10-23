#pragma once
#include "interface_intern.h"
/* Sensor fit */
enum {
	CAMERA_SENSOR_FIT_AUTO = 0,
	CAMERA_SENSOR_FIT_HOR = 1,
	CAMERA_SENSOR_FIT_VERT = 2,
};
class camera
{
public:
	camera();
	~camera();
	void BKE_camera_params_init();
	void BKE_camera_params_from_view3d();
	void update();
	void setortho(bool bortho);
	bool getortho() { return is_ortho; }
	rctf getViewplane();
	float getclip_start() { return clip_start; }
	float getclip_end() { return clip_end; }
private:
	int BKE_camera_sensor_fit(int sensor_fit, float sizex, float sizey);



private:
	float winx;
	float winy;
	float dist;
private:
	bool is_ortho;
	float lens;
	float ortho_scale;
	float zoom;

	float shiftx;
	float shifty;
	float offsetx;
	float offsety;

	/* sensor */
	float sensor_x;
	float sensor_y;
	int sensor_fit;

	/* clipping */
	float clip_start;
	float clip_end;

	/* computed viewplane */
	float ycor;
	float viewdx;
	float viewdy;
	rctf viewplane;

	/* computed matrix */
	float winmat[4][4];

};