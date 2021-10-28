#pragma once
#include "math_matrix.h"
typedef float Mat4[4][4];
typedef float Mat3[3][3];
#define MATRIX_STACK_DEPTH 32
typedef struct MatrixStack {
	Mat4 stack[MATRIX_STACK_DEPTH];
	uint top;
} MatrixStack;

typedef struct GPUMatrixState {
	MatrixStack model_view_stack;
	MatrixStack projection_stack;

	bool dirty;

	/* TODO: cache of derived matrices (Normal, MVP, inverse MVP, etc)
	 * generate as needed for shaders, invalidate when original matrices change
	 *
	 * TODO: separate Model from View transform? Batches/objects have model,
	 * camera/eye has view & projection
	 */
} GPUMatrixState;


Mat4 Projection;
Mat4 ModelView;

const float(*GPU_matrix_projection_get(float m[4][4]))[4]
{
  if (m == NULL) {
	static Mat4 temp;
	m = temp;
  }

  mul_m4_m4m4(m, Projection, ModelView);
  return m;
}
void GPU_matrix_projection_set(const float m[4][4])
{
	copy_m4_m4(Projection, m);
}
const float(*GPU_matrix_model_view_get(float m[4][4]))[4]
{
  if (m == NULL) {
	static Mat4 temp;
	m = temp;
  }

  mul_m4_m4m4(m, Projection, ModelView);
  return m;
}
void GPU_matrix_model_view_set(const float m[4][4])
{
	copy_m4_m4(Projection, m);
}