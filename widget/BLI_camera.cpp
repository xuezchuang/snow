#include "BLI_camera.h"
#include "gl/glew.h"
#include <string.h>
#include "Matrices.h"
#include "vmath.h"

#include <Eigen/Core>
#include <Eigen/Dense>
using Eigen::Map;
using Eigen::Matrix4f;

void quat_to_mat4(float m[4][4], const float q[4])
{
	double q0, q1, q2, q3, qda, qdb, qdc, qaa, qab, qac, qbb, qbc, qcc;

	q0 = M_SQRT2 * (double)q[0];
	q1 = M_SQRT2 * (double)q[1];
	q2 = M_SQRT2 * (double)q[2];
	q3 = M_SQRT2 * (double)q[3];

	qda = q0 * q1;
	qdb = q0 * q2;
	qdc = q0 * q3;
	qaa = q1 * q1;
	qab = q1 * q2;
	qac = q1 * q3;
	qbb = q2 * q2;
	qbc = q2 * q3;
	qcc = q3 * q3;

	m[0][0] = (float)(1.0 - qbb - qcc);
	m[0][1] = (float)(qdc + qab);
	m[0][2] = (float)(-qdb + qac);
	m[0][3] = 0.0f;

	m[1][0] = (float)(-qdc + qab);
	m[1][1] = (float)(1.0 - qaa - qcc);
	m[1][2] = (float)(qda + qbc);
	m[1][3] = 0.0f;

	m[2][0] = (float)(qdb + qac);
	m[2][1] = (float)(-qda + qbc);
	m[2][2] = (float)(1.0 - qaa - qbb);
	m[2][3] = 0.0f;

	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}
void translate_m4(float mat[4][4], float Tx, float Ty, float Tz)
{
	mat[3][0] += (Tx * mat[0][0] + Ty * mat[1][0] + Tz * mat[2][0]);
	mat[3][1] += (Tx * mat[0][1] + Ty * mat[1][1] + Tz * mat[2][1]);
	mat[3][2] += (Tx * mat[0][2] + Ty * mat[1][2] + Tz * mat[2][2]);
}
void copy_m4_m4(float m1[4][4], const float m2[4][4])
{
	memcpy(m1, m2, sizeof(float[4][4]));
}
void mul_m4_m4m4_uniq(float R[4][4], const float A[4][4], const float B[4][4])
{
	R[0][0] = B[0][0] * A[0][0] + B[0][1] * A[1][0] + B[0][2] * A[2][0] + B[0][3] * A[3][0];
	R[0][1] = B[0][0] * A[0][1] + B[0][1] * A[1][1] + B[0][2] * A[2][1] + B[0][3] * A[3][1];
	R[0][2] = B[0][0] * A[0][2] + B[0][1] * A[1][2] + B[0][2] * A[2][2] + B[0][3] * A[3][2];
	R[0][3] = B[0][0] * A[0][3] + B[0][1] * A[1][3] + B[0][2] * A[2][3] + B[0][3] * A[3][3];

	R[1][0] = B[1][0] * A[0][0] + B[1][1] * A[1][0] + B[1][2] * A[2][0] + B[1][3] * A[3][0];
	R[1][1] = B[1][0] * A[0][1] + B[1][1] * A[1][1] + B[1][2] * A[2][1] + B[1][3] * A[3][1];
	R[1][2] = B[1][0] * A[0][2] + B[1][1] * A[1][2] + B[1][2] * A[2][2] + B[1][3] * A[3][2];
	R[1][3] = B[1][0] * A[0][3] + B[1][1] * A[1][3] + B[1][2] * A[2][3] + B[1][3] * A[3][3];

	R[2][0] = B[2][0] * A[0][0] + B[2][1] * A[1][0] + B[2][2] * A[2][0] + B[2][3] * A[3][0];
	R[2][1] = B[2][0] * A[0][1] + B[2][1] * A[1][1] + B[2][2] * A[2][1] + B[2][3] * A[3][1];
	R[2][2] = B[2][0] * A[0][2] + B[2][1] * A[1][2] + B[2][2] * A[2][2] + B[2][3] * A[3][2];
	R[2][3] = B[2][0] * A[0][3] + B[2][1] * A[1][3] + B[2][2] * A[2][3] + B[2][3] * A[3][3];

	R[3][0] = B[3][0] * A[0][0] + B[3][1] * A[1][0] + B[3][2] * A[2][0] + B[3][3] * A[3][0];
	R[3][1] = B[3][0] * A[0][1] + B[3][1] * A[1][1] + B[3][2] * A[2][1] + B[3][3] * A[3][1];
	R[3][2] = B[3][0] * A[0][2] + B[3][1] * A[1][2] + B[3][2] * A[2][2] + B[3][3] * A[3][2];
	R[3][3] = B[3][0] * A[0][3] + B[3][1] * A[1][3] + B[3][2] * A[2][3] + B[3][3] * A[3][3];
}
void mul_m4_m4_post(float R[4][4], const float B[4][4])
{
	float A[4][4];
	copy_m4_m4(A, R);
	mul_m4_m4m4_uniq(R, A, B);
}
void mul_m4_m4_pre(float R[4][4], const float A[4][4])
{
	float B[4][4];
	copy_m4_m4(B, R);
	mul_m4_m4m4_uniq(R, A, B);
}
void mul_m4_m4m4(float R[4][4], const float A[4][4], const float B[4][4])
{
	if (A == R) {
		mul_m4_m4_post(R, B);
	}
	else if (B == R) {
		mul_m4_m4_pre(R, A);
	}
	else {
		mul_m4_m4m4_uniq(R, A, B);
	}
}
bool EIG_invert_m4_m4(float inverse[4][4], const float matrix[4][4])
{
	Map<Matrix4f> M = Map<Matrix4f>((float*)matrix);
	Matrix4f R;
	bool invertible = true;
	M.computeInverseWithCheck(R, invertible, 0.0f);
	if (!invertible) {
		R = R.Zero();
	}
	memcpy(inverse, R.data(), sizeof(float) * 4 * 4);
	return invertible;
}
bool invert_m4_m4(float inverse[4][4], const float mat[4][4])
{
	return EIG_invert_m4_m4(inverse, mat);
}
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
#define CAMERA_PARAM_ZOOM_INIT_PERSP 3.0
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