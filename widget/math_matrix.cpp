#include "math_matrix.h"
#include <Eigen/Core>
#include <Eigen/Dense>
using Eigen::Map;
using Eigen::Matrix4f;

#ifndef M_PI
#  define M_PI 3.14159265358979323846 /* pi */
#endif
#ifndef M_PI_2
#  define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif
#ifndef M_SQRT2
#  define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif

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
void mul_v4d_m4v4d(double r[4], const float mat[4][4], const double v[4])
{
	const double x = v[0];
	const double y = v[1];
	const double z = v[2];

	r[0] = x * (double)mat[0][0] + y * (double)mat[1][0] + z * (double)mat[2][0] +
		(double)mat[3][0] * v[3];
	r[1] = x * (double)mat[0][1] + y * (double)mat[1][1] + z * (double)mat[2][1] +
		(double)mat[3][1] * v[3];
	r[2] = x * (double)mat[0][2] + y * (double)mat[1][2] + z * (double)mat[2][2] +
		(double)mat[3][2] * v[3];
	r[3] = x * (double)mat[0][3] + y * (double)mat[1][3] + z * (double)mat[2][3] +
		(double)mat[3][3] * v[3];
}

float mul_project_m4_v3_zfac(const float mat[4][4], const float co[3])
{
	return (mat[0][3] * co[0]) + (mat[1][3] * co[1]) + (mat[2][3] * co[2]) + mat[3][3];
}
void negate_v3_v3(float r[3], const float a[3])
{
	r[0] = -a[0];
	r[1] = -a[1];
	r[2] = -a[2];
}
void add_v3_v3(float r[3], const float a[3])
{
	r[0] += a[0];
	r[1] += a[1];
	r[2] += a[2];
}