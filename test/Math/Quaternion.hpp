#pragma once

////////////////////////////////////////////////////////////
// Includes ////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#include "Vector3f.hpp"
#include "Matrix3x3.hpp"
#include "MathUtility.hpp"
#include "Vectors.h"
#include "Matrices.h"

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

	void set(float _w, float _x, float _y, float _z)
	{
		w = _w;
		x = _x;
		y = _y;
		z = _z;
	}
	void        set(const Vector3& axis, float angle)
	{
		Vector3 v = axis;
		v.normalize();                  // convert to unit vector
		float sine = sinf(angle);       // angle is radian
		w = cosf(angle);
		x = v.x * sine;
		y = v.y * sine;
		z = v.z * sine;
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

	// static functions
	// return quaternion for rotation from v1 to v2
	static Quaternion getQuaternion(const Vector3& v1, const Vector3& v2);
	// return quaternion from Euler angles (x, y) or (x, y, z)
	// The rotation order is x->y->z.
	static Quaternion getQuaternion(const Vector2& angles);
	static Quaternion getQuaternion(const Vector3& angles);
	inline Quaternion(const Vector3& axis, float angle)
	{
		// angle is radian
		set(axis, angle);
	}
public:

	// operators
	Quaternion  operator-() const;                      // unary operator (negate)
	Quaternion  operator+(const Quaternion& rhs) const; // addition
	Quaternion  operator-(const Quaternion& rhs) const; // subtraction
	Quaternion  operator*(float a) const;               // scalar multiplication
	//Quaternion  operator*(const Quaternion& rhs) const; // multiplication
	Quaternion  operator*(const Vector3& v) const;      // conjugation for rotation
	Quaternion& operator+=(const Quaternion& rhs);      // addition
	Quaternion& operator-=(const Quaternion& rhs);      // subtraction
	Quaternion& operator*=(float a);                    // scalar multiplication
	Quaternion& operator*=(const Quaternion& rhs);      // multiplication
	//bool        operator==(const Quaternion& rhs) const;// exact comparison
	bool        operator!=(const Quaternion& rhs) const;// exact comparison

	inline Quaternion& operator=(const Quaternion& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }

	inline Quaternion operator*(const Quaternion& other) const { return Quaternion::Multiply(*this, other); }

	inline bool operator==(const Quaternion& other) const { return (x == other.x && y == other.y && z == other.z && w == other.w); }
	//inline bool operator!=(const Quaternion& other) const { return (x != other.x || y != other.y || z != other.z || w != other.w); }

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
		float w = sqrt(1.0f + m(0, 0) + m(1, 1) + m(2, 2)) * 0.5f;
		float scaler = 1.0f / (w * 4.0f);
		float x = (m(2, 1) - m(1, 2)) * scaler;
		float y = (m(0, 2) - m(2, 0)) * scaler;
		float z = (m(1, 0) - m(0, 1)) * scaler;
		return Quaternion(x, y, z, w);
	}

	static Vector3f RotateVector(const Quaternion& quat, const Vector3f& pos)
	{
		Quaternion quatPos = Quaternion(pos.x, pos.y, pos.z, 0.0f);
		Quaternion quatRotated = quat * quatPos * Quaternion::Conjugate(quat);
		return Vector3f(quatRotated.x, quatRotated.y, quatRotated.z);
	}

	Matrix4     getMatrix() const;                      // return as 4x4 matrix
public:

	float x, y, z, w;

public:

	static const Quaternion Identity;

};



inline Matrix4 Quaternion::getMatrix() const
{
	// NOTE: assume the quaternion is unit length
	// compute common values
	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;
	float xx2 = x * x2;
	float xy2 = x * y2;
	float xz2 = x * z2;
	float yy2 = y * y2;
	float yz2 = y * z2;
	float zz2 = z * z2;
	float sx2 = w * x2;
	float sy2 = w * y2;
	float sz2 = w * z2;

	// build 4x4 matrix (column-major) and return
	return Matrix4(1 - (yy2 + zz2), xy2 + sz2, xz2 - sy2, 0,
		xy2 - sz2, 1 - (xx2 + zz2), yz2 + sx2, 0,
		xz2 + sy2, yz2 - sx2, 1 - (xx2 + yy2), 0,
		0, 0, 0, 1);

	// for non-unit quaternion
	// ss+xx-yy-zz, 2xy+2sz,     2xz-2sy,     0
	// 2xy-2sz,     ss-xx+yy-zz, 2yz-2sx,     0
	// 2xz+2sy,     2yz+2sx,     ss-xx-yy+zz, 0
	// 0,           0,           0,           1

}
///////////////////////////////////////////////////////////////////////////////
// static functions
///////////////////////////////////////////////////////////////////////////////
inline Quaternion Quaternion::getQuaternion(const Vector3& v1, const Vector3& v2)
{
	const float EPSILON = 0.001f;
	const float HALF_PI = 1.570796f;

	// if two vectors are equal return the vector with 0 rotation
	if (v1.equal(v2, EPSILON))
	{
		return Quaternion(v1, 0);
	}
	// if two vectors are opposite return a perpendicular vector with 180 angle
	else if (v1.equal(-v2, EPSILON))
	{
		Vector3 v;
		if (v1.x > -EPSILON && v1.x < EPSILON)       // if x ~= 0
			v.set(1, 0, 0);
		else if (v1.y > -EPSILON && v1.y < EPSILON)  // if y ~= 0
			v.set(0, 1, 0);
		else                                        // if z ~= 0
			v.set(0, 0, 1);
		return Quaternion(v, HALF_PI);
	}

	Vector3 u1 = v1;                    // convert to normal vector
	Vector3 u2 = v2;
	u1.normalize();
	u2.normalize();

	Vector3 v = u1.cross(u2);           // compute rotation axis
	float angle = acosf(u1.dot(u2));    // rotation angle
	return Quaternion(v, angle * 0.5f);   // return half angle for quaternion
}



inline Quaternion Quaternion::getQuaternion(const Vector2& angles)
{
	Quaternion qx = Quaternion(Vector3(1, 0, 0), angles.x);   // rotate along X
	Quaternion qy = Quaternion(Vector3(0, 1, 0), angles.y);   // rotate along Y
	return qx * qy; // order: y->x
}



inline Quaternion Quaternion::getQuaternion(const Vector3& angles)
{
	Quaternion qx = Quaternion(Vector3(1, 0, 0), angles.x);   // rotate along X
	Quaternion qy = Quaternion(Vector3(0, 1, 0), angles.y);   // rotate along Y
	Quaternion qz = Quaternion(Vector3(0, 0, 1), angles.z);   // rotate along Z
	return qx * qy * qz;    // order: z->y->x

}

inline Quaternion Quaternion::operator-() const
{
	return Quaternion(-w, -x, -y, -z);
}



inline Quaternion Quaternion::operator+(const Quaternion& rhs) const
{
	return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}



inline Quaternion Quaternion::operator-(const Quaternion& rhs) const
{
	return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
}



inline Quaternion Quaternion::operator*(float a) const
{
	return Quaternion(a * w, a * x, a * y, a * z);
}


//
//inline Quaternion Quaternion::operator*(const Quaternion& rhs) const
//{
//	Vector3 v1(x, y, z);
//	Vector3 v2(rhs.x, rhs.y, rhs.z);
//
//	Vector3 cross = v1.cross(v2);                   // v x v'
//	float dot = v1.dot(v2);                         // v . v'
//	Vector3 v3 = cross + (s * v2) + (rhs.s * v1);   // v x v' + sv' + s'v
//
//	return Quaternion(s * rhs.s - dot, v3.x, v3.y, v3.z);
//}



inline Quaternion Quaternion::operator*(const Vector3& v) const
{
	Quaternion q(0, x, y, z);
	return *this * q;
}



inline Quaternion& Quaternion::operator+=(const Quaternion& rhs)
{
	w += rhs.w;  x += rhs.x;  y += rhs.y;  z += rhs.z;
	return *this;
}



inline Quaternion& Quaternion::operator-=(const Quaternion& rhs)
{
	w -= rhs.w;  x -= rhs.x;  y -= rhs.y;  z -= rhs.z;
	return *this;
}



inline Quaternion& Quaternion::operator*=(float a)
{
	w *= a;  x *= a;  y *= a; z *= a;
	return *this;
}



inline Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	Vector3 v1(x, y, z);
	Vector3 v2(rhs.x, rhs.y, rhs.z);

	Vector3 cross = v1.cross(v2);                   // v x v'
	float dot = v1.dot(v2);                         // v . v'
	Vector3 v3 = cross + (w * v2) + (rhs.w * v1);   // v x v' + sv' + s'v

	w = w * rhs.w - dot;
	x = v3.x;
	y = v3.y;
	z = v3.z;
	return *this;
}



//inline bool Quaternion::operator==(const Quaternion& rhs) const
//{
//	return (s == rhs.s) && (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
//}



inline bool Quaternion::operator!=(const Quaternion& rhs) const
{
	return (w != rhs.w) || (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

