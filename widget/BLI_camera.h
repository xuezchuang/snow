#pragma once
#include "interface_intern.h"

/* Sensor fit */
enum {
	CAMERA_SENSOR_FIT_AUTO = 0,
	CAMERA_SENSOR_FIT_HOR = 1,
	CAMERA_SENSOR_FIT_VERT = 2,
};
class BLICamera
{
public:
	BLICamera();
	~BLICamera();
	void BKE_camera_params_init();
	void BKE_camera_params_from_view3d();
	void update();
	void setortho(bool bortho);
	bool getortho() { return is_ortho; }
	rctf getViewplane();
	float getclip_start() { return clip_start; }
	float getclip_end() { return clip_end; }
	
	void getviewmatrix(float m1[4][4]);
	void ProcessMouseScroll(float offset);
	void ED_view3d_update_viewmat();
	float ED_view3d_calc_zfac(bool* r_flip);
	void ED_view3d_win_to_delta(const float mval[2]);
private:
	int BKE_camera_sensor_fit(int sensor_fit, float sizex, float sizey);
	void view3d_viewmatrix_set();
	
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

	///* computed matrix */
	//float winmat[4][4];






	/** View rotation, must be kept normalized. */
	float viewquat[4];
	/**
	 * View center & orbit pivot, negative of worldspace location,
	 * also matches -viewinv[3][0:3] in ortho mode.
	 */
	float ofs[3];
	/** GL_PROJECTION matrix. */
	float winmat[4][4];
	/** GL_MODELVIEW matrix. */
	float viewmat[4][4];
	/** Viewmat*winmat. */
	float persmat[4][4];
	/** Inverse of persmat. */
	float persinv[4][4];
	//
private:
	float zfac;//calc 

};