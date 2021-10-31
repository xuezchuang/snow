#pragma once
#include "gl/glew.h"
#include "BLI_sys_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/** float rectangle. */
typedef struct rctf {
	float xmin, xmax;
	float ymin, ymax;
} rctf;

/** integer rectangle. */
typedef struct rcti {
	int xmin, xmax;
	int ymin, ymax;
} rcti;

/* flags to set which corners will become rounded:
 *
 * 1------2
 * |      |
 * 8------4 */

enum {
	UI_CNR_TOP_LEFT = 1 << 0,
	UI_CNR_TOP_RIGHT = 1 << 1,
	UI_CNR_BOTTOM_RIGHT = 1 << 2,
	UI_CNR_BOTTOM_LEFT = 1 << 3,
	/* just for convenience */
	UI_CNR_NONE = 0,
	UI_CNR_ALL = (UI_CNR_TOP_LEFT | UI_CNR_TOP_RIGHT | UI_CNR_BOTTOM_RIGHT | UI_CNR_BOTTOM_LEFT),
};

/* uiBlock->direction */
enum {
	UI_DIR_UP = 1 << 0,
	UI_DIR_DOWN = 1 << 1,
	UI_DIR_LEFT = 1 << 2,
	UI_DIR_RIGHT = 1 << 3,
	UI_DIR_CENTER_X = 1 << 4,
	UI_DIR_CENTER_Y = 1 << 5,

	UI_DIR_ALL = UI_DIR_UP | UI_DIR_DOWN | UI_DIR_LEFT | UI_DIR_RIGHT,
};

/* Widget shader parameters, must match the shader layout. */
typedef struct uiWidgetBaseParameters {
	rctf recti, rect;
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

typedef struct UserDef {
	///** UserDef has separate do-version handling, and can be read from other files. */
	//int versionfile, subversionfile;
	/** Private, defaults to 20 for 72 DPI setting. */
	short widget_unit;
	///** Runtime, line width and point size based on DPI. */
	float pixelsize;
}UserDef;

/* from blenkernel blender.c */
extern UserDef U;


#pragma region math_base_inline.c
/* little macro so inline keyword works */
#if defined(_MSC_VER)
#  define BLI_INLINE static __forceinline
#else
#  define BLI_INLINE static inline __attribute__((always_inline)) __attribute__((__unused__))
#endif
#if BLI_MATH_DO_INLINE
#  ifdef _MSC_VER
#    define MINLINE static __forceinline
#    define MALWAYS_INLINE MINLINE
#  else
#    define MINLINE static inline
#    define MALWAYS_INLINE static inline __attribute__((always_inline)) __attribute__((unused))
#  endif
#else
#  define MINLINE
#  define MALWAYS_INLINE
#endif
MINLINE int min_ii(int a, int b);
//BLI_INLINE int BLI_rcti_size_x(const struct rcti* rct);
//BLI_INLINE int BLI_rcti_size_y(const struct rcti* rct);
//int BLI_rcti_size_x(const struct rcti* rct);
//int BLI_rcti_size_y(const struct rcti* rct);

float BLI_rcti_cent_x_fl(const struct rctf* rct);
float BLI_rcti_cent_y_fl(const struct rctf* rct);
//void BLI_rctf_rcti_copy(rctf* dst, const rcti* src);
//void BLI_rctf_sanitize(rctf* rect);
//void BLI_rctf_init(rctf* rect, float xmin, float xmax, float ymin, float ymax);


#pragma endregion math_base_inline.c

#ifdef __cplusplus
}
#endif
