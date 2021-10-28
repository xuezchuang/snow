#pragma once

void quat_to_mat4(float m[4][4], const float q[4]);
void translate_m4(float mat[4][4], float Tx, float Ty, float Tz);
void copy_m4_m4(float m1[4][4], const float m2[4][4]);
void mul_m4_m4m4_uniq(float R[4][4], const float A[4][4], const float B[4][4]);
void mul_m4_m4_post(float R[4][4], const float B[4][4]);
void mul_m4_m4_pre(float R[4][4], const float A[4][4]);
void mul_m4_m4m4(float R[4][4], const float A[4][4], const float B[4][4]);
bool EIG_invert_m4_m4(float inverse[4][4], const float matrix[4][4]);
bool invert_m4_m4(float inverse[4][4], const float mat[4][4]);
void mul_v4d_m4v4d(double r[4], const float mat[4][4], const double v[4]);

float mul_project_m4_v3_zfac(const float mat[4][4], const float co[3]);
void negate_v3_v3(float r[3], const float a[3]);
void add_v3_v3(float r[3], const float a[3]);