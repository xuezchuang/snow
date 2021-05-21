#ifndef __INTERFACE_INTERN_H__
#define __INTERFACE_INTERN_H__

/** float rectangle. */
typedef struct rctf {
	float xmin, xmax;
	float ymin, ymax;
} rctf;

/* Widget shader parameters, must match the shader layout. */
typedef struct uiWidgetBaseParameters {
	rctf rect, recti;
	float radi, rad;
	float facxi, facyi;
	float round_corners[4];
	float color_inner1[4], color_inner2[4];
	float color_outline[4], color_emboss[4];
	float color_tria[4];
	float tria1_center[2], tria2_center[2];
	float tria1_size, tria2_size;
	float shade_dir;
	/* We pack alpha check and discard factor in alpha_discard.
	 * If the value is negative then we do alpha check.
	 * The absolute value itself is the discard factor.
	 * Initialize value to 1.0.f if you don't want discard */
	float alpha_discard;
	float tria_type;
	float _pad[3];
} uiWidgetBaseParameters;

#endif