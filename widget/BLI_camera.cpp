#include "BLI_camera.h"
#include "gl/glew.h"
#include <string.h>
#include "vmath.h"
#include "math_matrix.h"
//-------------------------------------------//
BLICamera::BLICamera()
{

}

BLICamera::~BLICamera()
{}

int BLICamera::BKE_camera_sensor_fit(int sensor_fit, float sizex, float sizey)
{
	if (sensor_fit == CAMERA_SENSOR_FIT_AUTO) {
		if (sizex >= sizey) {
			return CAMERA_SENSOR_FIT_HOR;
		}
		else {
			return CAMERA_SENSOR_FIT_VERT;
		}
	}

	return sensor_fit;
}

#define DEFAULT_SENSOR_WIDTH 36.0f
#define DEFAULT_SENSOR_HEIGHT 24.0f

void BLICamera::BKE_camera_params_init()
{
	/* defaults */
	sensor_x = DEFAULT_SENSOR_WIDTH;
	sensor_y = DEFAULT_SENSOR_HEIGHT;
	sensor_fit = CAMERA_SENSOR_FIT_AUTO;

	zoom = 1.0f;

	/* fallback for non BLICamera objects */
	clip_start = 0.1f;
	clip_end = 100.0f;
	float viewport_size[4];
	glGetFloatv(GL_VIEWPORT, viewport_size);
	winx = viewport_size[2];
	winy = viewport_size[3];
	dist = 10.0f;// 1000.0f;
    lens = 50.0f;
	ofs[0] = ofs[1] = ofs[2] = 0.0;
}

/* values for CameraParams.zoom, need to be taken into account for some operations */
#define CAMERA_PARAM_ZOOM_INIT_CAMOB 1.0f
#define CAMERA_PARAM_ZOOM_INIT_PERSP 2.0
void BLICamera::BKE_camera_params_from_view3d()
{
    clip_end = 1000.0f;
    clip_start = 0.01f;
    if (is_ortho) 
    {
		/* orthographic view */
		float sensor_size = sensor_fit == CAMERA_SENSOR_FIT_VERT ? sensor_y : sensor_x;
		/* Halve, otherwise too extreme low zbuffer quality. */
		clip_end *= 0.5f;
		clip_start = -clip_end;

		is_ortho = true;
		/* make sure any changes to this match ED_view3d_radius_to_dist_ortho() */
		ortho_scale = dist * sensor_size / lens;
		zoom = CAMERA_PARAM_ZOOM_INIT_PERSP;
	}
	else {
		/* perspective view */
		zoom = CAMERA_PARAM_ZOOM_INIT_PERSP;
	}
}
void BLICamera::setortho(bool bortho)
{
    is_ortho = bortho;
}
void BLICamera::update()
{
    //rctf viewplane;
    float pixsize, viewfac, sensor_size, dx, dy;
    int _sensor_fit;
    BKE_camera_params_from_view3d();
    float yasp = 1.0;
    float xasp = 1.0;
    ycor = yasp / xasp;

    if (is_ortho) {
        pixsize = ortho_scale;
    }
    else {
        /* perspective BLICamera */
        sensor_size = sensor_fit == CAMERA_SENSOR_FIT_VERT ? sensor_y : sensor_x;
        pixsize = (sensor_size * clip_start) / lens;
    }

    /* determine sensor fit */
    _sensor_fit = BKE_camera_sensor_fit(sensor_fit, xasp * winx, yasp * winy);

    if (_sensor_fit == CAMERA_SENSOR_FIT_HOR) {
        viewfac = winx;
    }
    else {
        viewfac = ycor * winy;
    }

    pixsize /= viewfac;

    /* extra zoom factor */
    pixsize *= zoom;

    /* compute view plane:
     * fully centered, zbuffer fills in jittered between -.5 and +.5 */
    viewplane.xmin = -0.5f * (float)winx;
    viewplane.ymin = -0.5f * ycor * (float)winy;
    viewplane.xmax = 0.5f * (float)winx;
    viewplane.ymax = 0.5f * ycor * (float)winy;

    /* lens shift and offset */
    dx = 0.0;// shiftx* viewfac;// +winx * offsetx;
    dy = 0.0;// shifty* viewfac;// +winy * offsety;

    viewplane.xmin += dx;
    viewplane.ymin += dy;
    viewplane.xmax += dx;
    viewplane.ymax += dy;

    /* the window matrix is used for clipping, and not changed during OSA steps */
    /* using an offset of +0.5 here would give clip errors on edges */
    viewplane.xmin *= pixsize;

    viewplane.xmax *= pixsize;
    viewplane.ymin *= pixsize;
    viewplane.ymax *= pixsize;

    /* Used for rendering (offset by near-clip with perspective views), passed to RE_SetPixelSize.
     * For viewport drawing 'RegionView3D.pixsize'. */
    viewdx = pixsize;
    viewdy = ycor * pixsize;

	vmath::mat4 projection_matrix = vmath::frustum(viewplane.xmin, viewplane.xmax, viewplane.ymin, viewplane.ymax, 0.01f, 1000.0f);
	memcpy(winmat, projection_matrix, sizeof(float[4][4]));
}
rctf BLICamera::getViewplane()
{
    return viewplane;
}

void BLICamera::view3d_viewmatrix_set()
{
	viewquat[0] = 1.0; viewquat[1] = viewquat[2] = viewquat[3] = 0.0;
	quat_to_mat4(viewmat, viewquat);
	viewmat[3][2] -= dist;
	translate_m4(viewmat, ofs[0], ofs[1], ofs[2]);
}
void BLICamera::getviewmatrix(float m1[4][4])
{
	memcpy(m1, viewmat, sizeof(float[4][4]));
}
void BLICamera::ProcessMouseScroll(float offset)
{
	dist += offset;
}
void BLICamera::ED_view3d_update_viewmat()
{
	view3d_viewmatrix_set();
	mul_m4_m4m4(persmat, winmat, viewmat);
	invert_m4_m4(persinv, persmat);
}

float BLICamera::ED_view3d_calc_zfac(bool* r_flip)
{
	float co[3];
	negate_v3_v3(co, ofs);
	zfac = mul_project_m4_v3_zfac((float(*)[4])persmat, co);

	if (r_flip) {
		*r_flip = (zfac < 0.0f);
	}

	/* if x,y,z is exactly the viewport offset, zfac is 0 and we don't want that
	 * (accounting for near zero values) */
	if (zfac < 1.e-6f && zfac > -1.e-6f) {
		zfac = 1.0f;
	}

	/* Negative zfac means x, y, z was behind the camera (in perspective).
	 * This gives flipped directions, so revert back to ok default case. */
	if (zfac < 0.0f) {
		zfac = -zfac;
	}

	return zfac;
}
/**
 * Calculate a 3d difference vector from 2d window offset.
 * note that #ED_view3d_calc_zfac() must be called first to determine
 * the depth used to calculate the delta.
 * \param region: The region (used for the window width and height).
 * \param mval: The area relative 2d difference (such as event->mval[0] - other_x).
 * \param out: The resulting world-space delta.
 */
void BLICamera::ED_view3d_win_to_delta(const float mval[2])
{
	float out[3];
	float dx, dy;

	dx = 2.0f * mval[0] * zfac / winx;
	dy = 2.0f * mval[1] * zfac / winy;

	out[0] = (persinv[0][0] * dx + persinv[1][0] * dy);
	out[1] = (persinv[0][1] * dx + persinv[1][1] * dy);
	out[2] = (persinv[0][2] * dx + persinv[1][2] * dy);
	add_v3_v3(ofs, out);
}