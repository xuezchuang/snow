#pragma once

////////////////////////////////////////////////////////////
// Includes ////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#include "Vector3f.hpp"
#include "Matrix3x3.hpp"
#include "MathUtility.hpp"

////////////////////////////////////////////////////////////
// Classes /////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class Quaternion
{
public:

	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
		// DO NOTHING
	}

	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{
		// DO NOTHING
	}

	Quaternion(const Vector3f& axis, float radian)
	{
		float theta = radian * 0.5f;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		x = axis.x * sinTheta;
		y = axis.y * sinTheta;
		z = axis.z * sinTheta;
		w = cosTheta;
	}

	Quaternion(const Vector3f& from, const Vector3f& to)
	{
		(*this) = FromTo(from, to);
	}

	~Quaternion()
	{
		// DO NOTHING
	}

public:

	inline Quaternion& operator=(const Quaternion& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }

	inline Quaternion operator*(const Quaternion& other) const { return Quaternion::Multiply(*this, other); }

	inline bool operator==(const Quaternion& other) const { return (x == other.x && y == other.y && z == other.z && w == other.w); }
	inline bool operator!=(const Quaternion& other) const { return (x != other.x || y != other.y || z != other.z || w != other.w); }

public:

	static Quaternion Multiply(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion
		(
			(q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
			(q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x),
			(q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w),
			(q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z)
		);
	}

	static Quaternion FromTo(const Vector3f& from, const Vector3f& to)
	{
		Matrix3x3 matrixFromTo = Matrix3x3::FromTo(from, to);
		return MatrixToQuaternion(matrixFromTo);
	}

	static Quaternion FromTo_Up(const Vector3f& from, const Vector3f& to)
	{
		Matrix3x3 matrixFromTo = Matrix3x3::FromTo_Up(from, to);
		return MatrixToQuaternion(matrixFromTo);
	}

	static Quaternion AxisRadian(const Vector3f& axis, float radian)
	{
		return Quaternion(axis, radian);
	}

	static Quaternion Conjugate(const Quaternion& q)
	{
		return Quaternion(-q.x, -q.y, -q.z, q.w);
	}

	static Quaternion MatrixToQuaternion(Matrix3x3& m)
	{

		float m11 = m(0, 0);
		float m12 = m(0, 1);
		float m13 = m(0, 2);
		//
		float m21 = m(1, 0);
		float m22 = m(1, 1);
		float m23 = m(1, 2);
		//
		float m31 = m(2, 0);
		float m32 = m(2, 1);
		float m33 = m(2, 2);

		float w, x, y, z;


		//探测四元数中最大的项 
		float fourWSquaredMinusl = m11 + m22 + m33;
		float fourXSquaredMinusl = m11 - m22 - m33;
		float fourYSquaredMinusl = m22 - m11 - m33;
		float fourZSquaredMinusl = m33 - m11 - m22;

		int biggestIndex = 0;
		float fourBiggestSqureMinus1 = fourWSquaredMinusl;
		if (fourXSquaredMinusl > fourBiggestSqureMinus1) {
			fourBiggestSqureMinus1 = fourXSquaredMinusl;
			biggestIndex = 1;
		}
		if (fourYSquaredMinusl > fourBiggestSqureMinus1) {
			fourBiggestSqureMinus1 = fourYSquaredMinusl;
			biggestIndex = 2;
		}
		if (fourZSquaredMinusl > fourBiggestSqureMinus1) {
			fourBiggestSqureMinus1 = fourZSquaredMinusl;
			biggestIndex = 3;
		}

		//计算平方根和除法 
		float biggestVal = sqrt(fourBiggestSqureMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		//计算四元数的值
		switch (biggestIndex) {
		case 0:
			w = biggestVal;
			x = (m23 - m32) * mult;
			y = (m31 - m13) * mult;
			z = (m12 - m21) * mult;
			break;
		case 1:
			x = biggestVal;
			w = (m23 - m32) * mult;
			y = (m12 + m21) * mult;
			z = (m31 + m13) * mult;
			break;
		case 2:
			y = biggestVal;
			w = (m31 - m13) * mult;
			x = (m12 + m21) * mult;
			z = (m23 + m32) * mult;
			break;
		case 3:
			z = biggestVal;
			w = (m12 - m21) * mult;
			x = (m31 + m13) * mult;
			y = (m23 + m32) * mult;
			break;
		}
		//float w = sqrt(1.0f + m(0, 0) + m(1, 1) + m(2, 2)) * 0.5f;
		//float scaler = 1.0f / (w * 4.0f);
		//float x = (m(2, 1) - m(1, 2)) * scaler;
		//float y = (m(0, 2) - m(2, 0)) * scaler;
		//float z = (m(1, 0) - m(0, 1)) * scaler;
		return Quaternion(x, y, z, w);
	}

	static Vector3f RotateVector(const Quaternion& quat, const Vector3f& pos)
	{
		Quaternion quatPos = Quaternion(pos.x, pos.y, pos.z, 0.0f);
		Quaternion quatRotated = (quat * quatPos) * Quaternion::Conjugate(quat);
		return Vector3f(quatRotated.x, quatRotated.y, quatRotated.z);
	}

public:

	float x, y, z, w;

public:

	static const Quaternion Identity;

};

const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 1.0f, 0.0f);