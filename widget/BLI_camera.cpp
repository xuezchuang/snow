#include "BLI_camera.h"
#include "gl/glew.h"

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
    dist = 1000.0f;
    lens = 5.0f;
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
}
rctf BLICamera::getViewplane()
{
    return viewplane;
}