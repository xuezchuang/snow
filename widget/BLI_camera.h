#pragma once
#include "interface_intern.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846 /* pi */
#endif
#ifndef M_PI_2
#  define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif
#ifndef M_SQRT2
#  define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif


bool invert_m4_m4(float inverse[4][4], const float mat[4][4]);
void mul_v4d_m4v4d(double r[4], const float mat[4][4], const double v[4]);
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
	float ED_view3d_calc_zfac(const float co[3], bool* r_flip);
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

};