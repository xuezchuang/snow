//Protection
#pragma once


#define T_DATA_EPSILON 1.0E-12f
#define _2PI_		6.283185307179586476925286766559
#define _PI_        _2PI_*0.5
#define _05PI_      _PI_*0.5

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define DECLARE_TMPL_API template<typename T>
#define DECLARE_TMPL2_API template<typename T, typename T2>
#define DECLARE_TMPL3_API template<typename T, typename T2, typename T3>

//////////////////////////////////////////////////////////////////////////
namespace Aries_TmplMath
{
	DECLARE_TMPL2_API void setVector(const T v[3], T2 vout[3])
	{
		vout[0] = T2(v[0]);	vout[1] = T2(v[1]);	vout[2] = T2(v[2]);
	}

	DECLARE_TMPL_API T getLength(T vx, T vy, T vz)
	{
		T dblLength = vx * vx + vy * vy + vz * vz;
		if (T_DATA_EPSILON > fabs(dblLength)) return 0.f;
		return sqrt(dblLength);
	}

	DECLARE_TMPL_API T getLength2D(T vx, T vy)
	{
		T dblLength = vx * vx + vy * vy;
		if (T_DATA_EPSILON > fabs(dblLength)) return 0.f;
		return sqrt(dblLength);
	}

	DECLARE_TMPL_API T getLength(const T v[3])
	{
		return getLength(v[0], v[1], v[2]);
	}

	DECLARE_TMPL2_API void getNormalize(T vx, T vy, T vz, T2 vout[3])
	{
		T dist = getLength(vx, vy, vz);
		dist = (dist < T_DATA_EPSILON) ? 1.f : 1.f / dist;
		vout[0] = T2(vx * dist);	vout[1] = T2(vy * dist);	vout[2] = T2(vz * dist);
	}

	DECLARE_TMPL2_API void getNormalize2D(T vx, T vy, T2 vout[2])
	{
		T dist = getLength2D(vx, vy);
		dist = (dist < T_DATA_EPSILON) ? 1.f : 1.f / dist;
		vout[0] = T2(vx * dist);	vout[1] = T2(vy * dist);
	}

	DECLARE_TMPL_API void getNormalize(T v[3])
	{
		getNormalize(v[0], v[1], v[2], v);
	}

	DECLARE_TMPL_API void getNormalize2D(T v[2])
	{
		getNormalize2D(v[0], v[1], v);
	}

	DECLARE_TMPL2_API void getNormalize(const T v[3], T2 vout[3])
	{
		getNormalize(v[0], v[1], v[2], vout);
	}

	//////////////////////////////////////////////////////////////////////////
	// Vector Operation
	DECLARE_TMPL2_API void getAdd(const T v0[3], const T v1[3], T2 vout[3])
	{
		vout[0] = T2(v0[0] + v1[0]);	vout[1] = T2(v0[1] + v1[1]);	vout[2] = T2(v0[2] + v1[2]);
	}

	DECLARE_TMPL2_API void getAdd2D(const T v0[2], const T v1[2], T2 vout[2])
	{
		vout[0] = T2(v0[0] + v1[0]);	vout[1] = T2(v0[1] + v1[1]);
	}

	DECLARE_TMPL2_API void getSub(const T v0[3], const T v1[3], T2 vout[3])
	{
		vout[0] = T2(v0[0] - v1[0]);	vout[1] = T2(v0[1] - v1[1]);	vout[2] = T2(v0[2] - v1[2]);
	}

	DECLARE_TMPL2_API void getSub2D(const T v0[2], const T v1[2], T2 vout[2])
	{
		vout[0] = T2(v0[0] - v1[0]);	vout[1] = T2(v0[1] - v1[1]);
	}

	DECLARE_TMPL2_API void getVect(const T pnt0[], const T pnt1[], T2 dest[])
	{
		dest[0] = T2(pnt1[0] - pnt0[0]);	dest[1] = T2(pnt1[1] - pnt0[1]);	dest[2] = T2(pnt1[2] - pnt0[2]);
	}

	DECLARE_TMPL3_API void getLerpS(T ra0, T ra1, T3 ratio, T2 dest)
	{
		dest = T2(ra0 + ratio * (ra1 - ra0));
	}

	DECLARE_TMPL3_API void getLerp(const T pnt0[3], const T pnt1[3], T3 ratio, T2 dest[3])
	{
		dest[0] = T2(pnt0[0] + ratio * (pnt1[0] - pnt0[0]));	dest[1] = T2(pnt0[1] + ratio * (pnt1[1] - pnt0[1]));	dest[2] = T2(pnt0[2] + ratio * (pnt1[2] - pnt0[2]));
	}

	DECLARE_TMPL_API T getDist(const T pnt0[], const T pnt1[])
	{
		T dblLength = (pnt0[0] - pnt1[0]) * (pnt0[0] - pnt1[0]) + (pnt0[1] - pnt1[1]) * (pnt0[1] - pnt1[1]) + (pnt0[2] - pnt1[2]) * (pnt0[2] - pnt1[2]);
		if (fabs(dblLength) < T_DATA_EPSILON) return 0.f;
		return sqrt(dblLength);
	}

	DECLARE_TMPL_API T getDist2D(const T pnt0[], const T pnt1[])
	{
		T dblLength = (pnt0[0] - pnt1[0]) * (pnt0[0] - pnt1[0]) + (pnt0[1] - pnt1[1]) * (pnt0[1] - pnt1[1]);
		if (fabs(dblLength) < T_DATA_EPSILON) return 0.f;
		return sqrt(dblLength);
	}

	DECLARE_TMPL_API T getDot(const T v0[3], const T v1[3])
	{
		return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]);
	}

	DECLARE_TMPL_API T getDot2D(const T v0[2], const T v1[2])
	{
		return (v0[0] * v1[0] + v0[1] * v1[1]);
	}

	DECLARE_TMPL2_API void getCross(const T v0[3], const T v1[3], T2 dest[3])
	{
		dest[0] = T2(v0[1] * v1[2] - v0[2] * v1[1]);	dest[1] = T2(v0[2] * v1[0] - v0[0] * v1[2]);	dest[2] = T2(v0[0] * v1[1] - v0[1] * v1[0]);
	}

	DECLARE_TMPL2_API void getCross2D(const T v0[2], const T v1[2], T2 dest[2])
	{
		dest[0] = T2(v0[1] - v1[1]);	dest[1] = T2(v1[0] - v0[0]);
	}

	DECLARE_TMPL2_API void getNormalVector(const T pnt0[3], const T pnt1[3], const T pnt2[3], T2 norm[3])
	{
		T v0[3], v1[3];
		getVect(pnt0, pnt1, v0);	getVect(pnt0, pnt2, v1);
		getCross(v0, v1, norm);	getNormalize(norm);
	}

	DECLARE_TMPL_API T getAngle(const T v0[3], const T v1[3])//ret. angle: Radian [0:pi]
	{
		const T Cos = getDot(v0, v1);
		return T(acos(CLAMP(Cos, -1.f, 1.f)));
	}

	DECLARE_TMPL_API T getAngle2D(const T v0[2], const T v1[2])//ret. angle: Radian [0:pi]
	{
		const T Cos = getDot2D(v0, v1);
		return T(acos(CLAMP(Cos, -1.f, 1.f)));
	}

	DECLARE_TMPL_API T getAngle(const T v0[3], const T v1[3], const T refDir[3])//ret. angle: Radian [0:2_pi]
	{
		const T angle = getAngle(v0, v1);
		T cross[3];	getCross(v0, v1, cross);
		if (getAngle(cross, refDir) > M_PI_2)
			return(M_PI * 2 - angle);
		return angle;

	}

	DECLARE_TMPL_API T IsLeft(const T pt1[2], const T pt2[2], const T pt3[2])
	{
		double dLeft = (pt2[0] - pt1[0]) * (pt3[1] - pt1[1]) - (pt3[0] - pt1[0]) * (pt2[1] - pt1[1]);
		if (dLeft == 0)
			return 0;
		return dLeft;
	}

	DECLARE_TMPL_API T getAngle2D(const T A[2], const T B[2], const T C[2])
	{
		double side_a = sqrt((B[0] - C[0]) * (B[0] - C[0]) + (B[1] - C[1]) * (B[1] - C[1]));
		double side_b = sqrt((A[0] - C[0]) * (A[0] - C[0]) + (A[1] - C[1]) * (A[1] - C[1]));
		double side_c = sqrt((A[0] - B[0]) * (A[0] - B[0]) + (A[1] - B[1]) * (A[1] - B[1]));

		double dAng = fabs(acos(((side_b * side_b) - (side_a * side_a) - (side_c * side_c)) / (-2 * side_a * side_c)));
		double dLeft = IsLeft(A, B, C);
		if (dLeft > 0)
			return dAng;
		else if (dLeft == 0)
			return M_PI;
		else
			return 2 * M_PI - dAng;
	}
	//////////////////////////////////////////////////////////////////////////
	// 
	DECLARE_TMPL_API void UCS2GCS_comp3(T x, T y, T z, const T ucs[3][3], T& destx, T& desty, T& destz)
	{
		destx = x * ucs[0][0] + y * ucs[1][0] + z * ucs[2][0];
		desty = x * ucs[0][1] + y * ucs[1][1] + z * ucs[2][1];
		destz = x * ucs[0][2] + y * ucs[1][2] + z * ucs[2][2];
	}

	DECLARE_TMPL_API void UCS2GCS_comp3(const T ucs[3][3], T& x, T& y, T& z)
	{
		UCS2GCS_comp3(x, y, z, ucs, x, y, z);
	}

	DECLARE_TMPL_API void UCS2GCS_comp3(const T xyz[3], const T ucs[3][3], T destxyz[3])
	{
		UCS2GCS_comp3(xyz[0], xyz[1], xyz[2], ucs, destxyz[0], destxyz[1], destxyz[2]);
	}

	DECLARE_TMPL_API void UCS2GCS_comp3(const T ucs[3][3], T xyz[3])
	{
		UCS2GCS_comp3(xyz, ucs, xyz);
	}

	DECLARE_TMPL_API void GCS2UCS_comp3(T x, T y, T z, const T ucs[3][3], T& destx, T& desty, T& destz)
	{
		destx = x * ucs[0][0] + y * ucs[0][1] + z * ucs[0][2];
		desty = x * ucs[1][0] + y * ucs[1][1] + z * ucs[1][2];
		destz = x * ucs[2][0] + y * ucs[2][1] + z * ucs[2][2];
	}

	DECLARE_TMPL_API void GCS2UCS_comp3(const T ucs[3][3], T& x, T& y, T& z)
	{
		GCS2UCS_comp3(x, y, z, ucs, x, y, z);
	}

	DECLARE_TMPL_API void GCS2UCS_comp3(const T xyz[3], const T ucs[3][3], T destxyz[3])
	{
		GCS2UCS_comp3(xyz[0], xyz[1], xyz[2], ucs, destxyz[0], destxyz[1], destxyz[2]);
	}

	DECLARE_TMPL_API void GCS2UCS_comp3(const T ucs[3][3], T xyz[3])
	{
		GCS2UCS_comp3(xyz, ucs, xyz);
	}

	DECLARE_TMPL_API void TransformVector(const T M[][4], const T src[], T dst[])
	{
		dst[0] = M[0][0] * src[0] + M[0][1] * src[1] + M[0][2] * src[2] + M[0][3];
		dst[1] = M[1][0] * src[0] + M[1][1] * src[1] + M[1][2] * src[2] + M[1][3];
		dst[2] = M[2][0] * src[0] + M[2][1] * src[1] + M[2][2] * src[2] + M[2][3];
	}

	DECLARE_TMPL_API void getRotMat(T x, T y, T z, T angle, T Mrot[][3])//angle: rad
	{
		const T c = cos(angle);	const T s = sin(angle);	const T ac = T(1.f - c);
		Mrot[0][0] = x * x * ac * c;	  Mrot[0][1] = x * y * ac + z * s;     Mrot[0][2] = x * z * ac - y * s;
		Mrot[1][0] = x * y * ac - z * s;  Mrot[1][1] = y * y * ac + c;       Mrot[1][2] = y * z * ac + x * s;
		Mrot[2][0] = x * z * ac + y * s;  Mrot[2][1] = y * z * ac - x * s;     Mrot[2][2] = z * z * ac + c;
	}

	DECLARE_TMPL_API void getRotMat(const T angle, const T axis[], T Mrot[][4])
	{
		double radian = double(angle) / 180. * 3.14159265358979323846;

		double c = double(cos(radian));
		double s = double(sin(radian));
		double ac = double(1.f - c);

		double dAxis[3] = { (double)axis[0], (double)axis[1], (double)axis[2] };
		double norm = sqrt(dAxis[0] * dAxis[0] + dAxis[1] * dAxis[1] + dAxis[2] * dAxis[2]);
		if (norm < 1.e-8)
		{
			for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) Mrot[i][j] = T(0.);
			Mrot[0][0] = Mrot[1][1] = Mrot[2][2] = Mrot[3][3] = T(1.);
			return;
		}
		dAxis[0] /= norm;
		dAxis[1] /= norm;
		dAxis[2] /= norm;

		double nxy = dAxis[0] * dAxis[1];
		double nyz = dAxis[1] * dAxis[2];
		double nzx = dAxis[2] * dAxis[0];

		double nxx = dAxis[0] * dAxis[0];
		double nyy = dAxis[1] * dAxis[1];
		double nzz = dAxis[2] * dAxis[2];

		double nx = dAxis[0];
		double ny = dAxis[1];
		double nz = dAxis[2];

		Mrot[0][0] = T(nxx * ac + c);
		Mrot[0][1] = T(nxy * ac - nz * s);
		Mrot[0][2] = T(nzx * ac + ny * s);
		Mrot[0][3] = T(0.f);

		Mrot[1][0] = T(nxy * ac + nz * s);
		Mrot[1][1] = T(nyy * ac + c);
		Mrot[1][2] = T(nyz * ac - nx * s);
		Mrot[1][3] = T(0.f);

		Mrot[2][0] = T(nzx * ac - ny * s);
		Mrot[2][1] = T(nyz * ac + nx * s);
		Mrot[2][2] = T(nzz * ac + c);
		Mrot[2][3] = T(0.f);

		Mrot[3][0] = Mrot[3][1] = Mrot[3][2] = T(0.f);
		Mrot[3][3] = T(1.f);
	}

	DECLARE_TMPL_API void rotate(T p[2], T angle)//angle: rad
	{
		const T c = cos(angle);	const T s = sin(angle);
		T x = p[0]; T y = p[1];
		p[0] = c * x - s * y;
		p[1] = s * x + c * y;
	}

	DECLARE_TMPL_API void rotate(T p[2], T angle, T o[2])//angle: rad
	{
		const T c = cos(angle);	const T s = sin(angle);
		o[0] = c * p[0] - s * p[1];
		o[1] = s * p[0] + c * p[1];
	}

	DECLARE_TMPL_API void getRotMat(const T axis[3], T angle, T Mrot[][3])//angle: rad
	{
		getRotMat(axis[0], axis[1], axis[2], angle, Mrot);
	}

	DECLARE_TMPL_API void getRotMatZ(T s, T c, T ax, T ay, T Mrot[][3])
	{
		T ac = T(1.f - c);
		T axy = ax * ay;
		Mrot[0][0] = ax * ax * ac + c;	Mrot[0][1] = axy * ac;		Mrot[0][2] = ay * s;
		Mrot[1][0] = axy * ac;		Mrot[1][1] = ay * ay * ac + c;	Mrot[1][2] = -ax * s;
		Mrot[2][0] = -ay * s;			Mrot[2][1] = ax * s;			Mrot[2][2] = c;
	}

	DECLARE_TMPL_API void getRotMat(const T vect[3], T Mrot[][3])//vect와 Z의 외적을 축으로 회전
	{
		const T vect2length = T(sqrt(vect[0] * vect[0] + vect[1] * vect[1]));
		if (T_DATA_EPSILON > vect2length)
		{
			memset(Mrot, 0, sizeof(T) * 9);
			Mrot[0][0] = Mrot[1][1] = Mrot[2][2] = 1.f;
			if (vect[2] < 0) Mrot[1][1] = Mrot[2][2] = -1.f;
		}
		else
		{
			const T denom2 = 1.f / vect2length;
			const T Cos = vect[2];
			const T angle = T(acos(CLAMP(Cos, -1.f, 1.f)));
			const T Sin = T(sin(angle));
			getRotMatZ(Sin, Cos, -vect[1] * denom2, vect[0] * denom2, Mrot);
		}
	}

	DECLARE_TMPL_API void getRotVectAngle(const T v[3], T axis[3], T& angle)//angle: angle
	{
		const T Cos = v[2];
		angle = T(DEG(acos(CLAMP(Cos, -1.f, 1.f))));

		const T vect2length = T(sqrt(v[0] * v[0] + v[1] * v[1]));
		if (T_DATA_EPSILON > vect2length)
		{
			axis[0] = 1.f; axis[1] = 0.f; axis[2] = 0.f;
		}
		else
		{
			T vz[3] = { 0.f, 0.f, 1.f };
			getCross(vz, v, axis);
		}
	}

	DECLARE_TMPL_API void getRotVectAngle(const T v[3], T axis[3], T& angleZ, T& angleX)
	{
		getRotVectAngle(v, axis, angleZ);

		if (fabs(axis[1]) < 1e-8)
		{
			if (axis[0] > 0.0)
			{
				angleX = 0.f;
			}
			else
			{
				angleX = 180.f;
			}
		}
		else
		{
			const T Cos = axis[0] / axis[1];
			angleX = T(DEG(acos(CLAMP(Cos, -1.f, 1.f))));
		}
	}

	DECLARE_TMPL_API BOOL getPlaneEquation(T p1[3], T dPlaneNormalVec[3], T& a, T& b, T& c, T& h)
	{
		a = 0., b = 0., c = 0., h = 0.;

		getNormalize(dPlaneNormalVec, dPlaneNormalVec);
		a = dPlaneNormalVec[0];
		b = dPlaneNormalVec[1];
		c = dPlaneNormalVec[2];
		h = -(a * p1[0] + b * p1[1] + c * p1[2]);
		return TRUE;
	}

	DECLARE_TMPL_API BOOL getPlaneEquation(const T p1[3], const T p2[3], const T p3[3], T& a, T& b, T& c, T& h)
	{
		a = 0., b = 0., c = 0., h = 0.;

		T aa, bb, cc, dd;
		aa = p1[1] * (p2[2] - p3[2]) - p2[1] * (p1[2] - p3[2]) + p3[1] * (p1[2] - p2[2]);
		bb = p1[0] * (p3[2] - p2[2]) + p2[0] * (p1[2] - p3[2]) + p3[0] * (p2[2] - p1[2]);
		cc = p1[0] * (p2[1] - p3[1]) - p2[0] * (p1[1] - p3[1]) + p3[0] * (p1[1] - p2[1]);
		dd = sqrt(aa * aa + bb * bb + cc * cc);
		if (dd < m_NormalZero)return FALSE;
		a = aa / dd;
		b = bb / dd;
		c = cc / dd;
		h = -(a * p1[0] + b * p1[1] + c * p1[2]);
		return TRUE;
	}

	DECLARE_TMPL_API bool project_on_plane_normal(const T node_xyz[3], const T a, const T b, const T c, const T d, const T tolerance, T projected_node_xyz[3])
	{
		T denominator = pow(a, 2.0) + pow(b, 2.0) + pow(c, 2.0);
		if (fabs(denominator) < tolerance) return false;
		T xi = (d - a * node_xyz[0] - b * node_xyz[1] - c * node_xyz[2]) / denominator;
		projected_node_xyz[0] = node_xyz[0] + xi * a;
		projected_node_xyz[1] = node_xyz[1] + xi * b;
		projected_node_xyz[2] = node_xyz[2] + xi * c;
		return true;
	} // end: project_on_plane_normal()

	DECLARE_TMPL_API bool project_on_plane_normal(const T node_xyz[3], const T plane_xyz1[3], const T plane_xyz2[2], const T plane_xyz3[3], const T tolerance, T projected_node_xyz[3])
	{
		T mf_vector12[3], mf_vector13[3];
		for (int i = 0; i < 3; i++)
		{
			mf_vector12[i] = plane_xyz2[i] - plane_xyz1[i];
			mf_vector13[i] = plane_xyz3[i] - plane_xyz1[i];
		}
		T a = mf_vector12[1] * mf_vector13[2] - mf_vector13[1] * mf_vector12[2];
		T b = mf_vector13[0] * mf_vector12[2] - mf_vector12[0] * mf_vector13[2];
		T c = mf_vector12[0] * mf_vector13[1] - mf_vector13[0] * mf_vector12[1];
		T d = a * plane_xyz1[0] + b * plane_xyz1[1] + c * plane_xyz1[2];
		T denominator = pow(a, 2.0) + pow(b, 2.0) + pow(c, 2.0);
		if (fabs(denominator) < tolerance) return false;
		T xi = (d - a * node_xyz[0] - b * node_xyz[1] - c * node_xyz[2]) / denominator;
		projected_node_xyz[0] = node_xyz[0] + xi * a;
		projected_node_xyz[1] = node_xyz[1] + xi * b;
		projected_node_xyz[2] = node_xyz[2] + xi * c;
		return true;
	} // end: project_on_plane_normal()

	DECLARE_TMPL_API HVector<T, 3> getCenterPnt(const T* apP1, const T* apP2, const T* apP3)
	{
		return tagTVector3<T>((apP1[0] + apP2[0] + apP3[0]) / 3.,
			(apP1[1] + apP2[1] + apP3[1]) / 3.,
			(apP1[2] + apP2[2] + apP3[2]) / 3.);
	}
	DECLARE_TMPL_API void getCenterPnt(const T* apP1, const T* apP2, const T* apP3, const T* apP4, T* apCenter)
	{
		apCenter[0] = (apP1[0] + apP2[0] + apP3[0] + apP4[0]) * .25;
		apCenter[1] = (apP1[1] + apP2[1] + apP3[1] + apP4[1]) * .25;
		apCenter[2] = (apP1[2] + apP2[2] + apP3[2] + apP4[2]) * .25;
	}

	DECLARE_TMPL_API HVector<T, 3> getCenterPnt(const T* apP1, const T* apP2, const T* apP3, const T* apP4)
	{
		T Center[3];
		getCenterPnt(apP1, apP2, apP3, apP4, Center);
		return tagTVector3<T>(Center[0], Center[1], Center[2]);
	}


	DECLARE_TMPL_API T AngleToRad(T dAngle)
	{
		T dVal = fabs(dAngle);
		if (dVal > 360.0)
		{
			long lIdx = (long)(dAngle / 360.0);
			if (dAngle < 0.0)
			{
				dAngle = dAngle + (double)(360.0 * lIdx);
			}
			else
			{
				dAngle = dAngle - (double)(360.0 * lIdx);
			}
		}

		return (dAngle * _2PI_) / 360.0;
	}

	DECLARE_TMPL_API T getDistanceLinePoint(const T pA[2], const T pB[2], const T pMP[2])
	{
		T a, b, c, d;   //pA-pB a, pB-pMP b, pA-pMP c, 법선-pMP d
		T rM;
		T C;     //ax + by + c = 0 에서 c
		a = sqrt(pow(pA[0] - pB[0], 2) + pow(pA[1] - pB[1], 2));
		b = sqrt(pow(pB[0] - pMP[0], 2) + pow(pB[1] - pMP[1], 2));
		c = sqrt(pow(pA[0] - pMP[0], 2) + pow(pA[1] - pMP[1], 2));

		if (pow(a, 2) + pow(b, 2) <= pow(c, 2))  //삼각형을 만들었을 때 둔각일 때
			return b;
		else if (pow(a, 2) + pow(c, 2) <= pow(b, 2))
			return c;

		//직선이 방정식이 안나오는 ( y= k 또는 x = k) 직선 일때
		if (pA[0] == pB[0])
		{
			d = pMP[0] - pA[0];
			return d;
		}
		if (pA[1] == pB[1])
		{
			d = pMP[1] - pA[1];
			return d;
		}

		rM = (T)(pA[1] - pB[1]) / (T)(pA[0] - pB[0]);
		C = pA[1] - rM * pA[0];
		d = abs((T)(rM * (T)pMP[0] - (T)pMP[1] + C)) / sqrt(pow(rM, 2) + 1);
		return d;
	}

	DECLARE_TMPL_API BOOL IsParallel(const T pV1[3], const T pV2[3], const T Tol)
	{
		T pVOut1[3], pVOut2[3];
		getNormalize(pV1, pVOut1);
		getNormalize(pV2, pVOut2);
		return wzTmplMath::getDist(pVOut1, pVOut2) < Tol;
	}

	DECLARE_TMPL_API void getNormalVector(const T pnt0[3], const T pnt1[3], T norm[3])
	{
		T Vector[3];
		Vector[0] = pnt0[1] * pnt1[2] - pnt0[2] * pnt1[1];
		Vector[1] = pnt0[2] * pnt1[0] - pnt0[0] * pnt1[2];
		Vector[2] = pnt0[0] * pnt1[1] - pnt0[1] * pnt1[0];
		norm[0] = Vector[0];
		norm[1] = Vector[1];
		norm[2] = Vector[2];
	}

	DECLARE_TMPL_API T getAngle2DWithSign(T v0[3], T v1[3], const T Tol)
	{
		getNormalize2D(v0);
		getNormalize2D(v1);
		T angle = getAngle2D(v0, v1);
		T VectorN[3];

		getNormalVector(v0, v1, VectorN);

		if (fabs(VectorN[2]) <= Tol) // Normal Vector가 XY 평면인 경우
		{
			if (fabs(VectorN[0]) <= Tol)  // X 가 0이면 Y 좌표 본다.
			{
				if (VectorN[1] >= 0.0) return angle;
				else return -angle;
			}
			else  // Y가 0이면 X좌표 본다.
			{
				if (VectorN[0] >= 0.0) return angle;
				else return -angle;
			}
		}

		if (VectorN[2] >= 0.0)  // CCW
			return angle;

		return -angle;			//CW
	}
}
