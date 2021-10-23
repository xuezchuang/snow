#pragma once

#include "../Math/Vector2f.hpp"
#include "../Math/Vector3f.hpp"
#include "../Math/Quaternion.hpp"
#include "../Math/MathUtility.hpp"

#include "../Scene/Ray.hpp"
#include "../Scene/Light.hpp"
#include "../Scene/Camera.hpp"
#include "../Scene/Scene.hpp"
#include "../Scene/SDFBox.hpp"
#include "../Scene/SDFPlane.hpp"
#include "../Scene/SDFSphere.hpp"
#include "../Scene/SDFUnion.hpp"
#include "../Scene/SDFSubtract.hpp"
#include "../Scene/SDFIntersect.hpp"

#include "../Image/Color3f.hpp"
#include "../Image/Texture2D.hpp"

#include "Raymarch.hpp"
#include "Matrices.h"
const float EPSILON = 0.00001f;
class RaymarchShading
{
public:

	RaymarchShading()
	{
		// TODO
	}
private:
	void lookAt(const Vector3& position, const Vector3& target)
	{
		Matrix4 matrix;
		Vector3 angle;
		// if pos and target are same, only translate camera to position without rotation
		if (position == target)
		{
			matrix.identity();
			matrix.setColumn(3, -position);
			// rotation stuff
			matrixRotation.identity();
			angle.set(0, 0, 0);
			//quaternion.set(1, 0, 0, 0);
			return;
		}

		Vector3 left, up, forward;  // 3 axis of matrix for scene

		// first, compute the forward vector of rotation matrix
		// NOTE: the direction is reversed (target to camera pos) because of camera transform
		forward = position - target;
		float distance = forward.length();  // remember the distance
		// normalize
		forward /= distance;

		// compute temporal up vector based on the forward vector
		// watch out when look up/down at 90 degree
		// for example, forward vector is on the Y axis
		if (fabs(forward.x) < EPSILON && fabs(forward.z) < EPSILON)
		{
			// forward vector is pointing +Y axis
			if (forward.y > 0)
			{
				up = Vector3(0, 0, -1);
			}
			// forward vector is pointing -Y axis
			else
			{
				up = Vector3(0, 0, 1);
			}
		}
		// in general, up vector is straight up
		else
		{
			up = Vector3(0, 1, 0);
		}

		// compute the left vector of rotation matrix
		left = up.cross(forward);   // cross product
		left.normalize();

		// re-calculate the orthonormal up vector
		up = forward.cross(left);   // cross product

		// set inverse rotation matrix: M^-1 = M^T for Euclidean transform
		matrixRotation.identity();

		matrixRotation.setRow(0, left);
		matrixRotation.setRow(1, up);
		matrixRotation.setRow(2, forward);
		matrixRotation.invert();
	}
	Matrix4 matrixRotation;
public:

	void Initialize(Texture2D& backbuffer)
	{
		// Camera
		_camera.m_Position = kCameraPosition;
		_camera.m_Rotation = Quaternion::FromTo(Vector3f::Forward, Vector3f::Normalize(-kCameraPosition));
		//_camera.m_Rotation = Quaternion::FromTo_Up(Vector3f::Forward, Vector3f::Normalize(-kCameraPosition));
		
		//
		Vector3 carm;
		carm.x = _camera.m_Position.x; carm.y = _camera.m_Position.y; carm.z = _camera.m_Position.z;
		lookAt(carm, Vector3(0, 0, 0));

		// Light
		_light.m_Color = kLightColor;
		_light.m_Position = kLightPosition;
		_light.m_Direction = Vector3f::Normalize(kLightPosition);

		// Plane
		_plane.m_Position = kPlanePosition;
		_plane.m_Normal = kPlaneNormal;
		_plane.m_Rotation = kPlaneRotation;

		// Box - Sphere
		_sphere.m_Radius = kSphereRadius;
		_sphere.m_Position = kSpherePosition;
		_sphere.m_Rotation = kSphereRotation;
		_box.m_Position = kBoxPosition;
		_box.m_Size = kBoxSize;
		_box.m_Rotation = kBoxRotation;
		_boxSubtractSphere.m_ObjectA = &_box;
		_boxSubtractSphere.m_ObjectB = &_sphere;
		_boxSubtractSphere.m_Rotation = kBoxSphereRotation;

		// Scene
		_scene.AddObject(&_plane);
		_scene.AddObject(&_boxSubtractSphere);
	}

	void Release(void)
	{
	}

	void Draw(Texture2D& backbuffer)
	{
		int iWindowWidth = backbuffer.GetWidth();
		int iWindowHeight = backbuffer.GetHeight();
		float fWindowWidth = (float)iWindowWidth;
		float fWindowHeight = (float)iWindowHeight;
		float fWindowRatio = fWindowWidth / fWindowHeight;

		Vector2f uv = Vector2f::Zero;
		for (int y = 0; y < iWindowHeight; ++y)
		{
			for (int x = 0; x < iWindowWidth; ++x)
			{
				uv.x = (float)x / fWindowWidth;
				uv.y = (float)y / fWindowHeight;
				backbuffer.Pixel(x, y) = FragmentFunc(uv, fWindowRatio);
			}
		}
	}

private:

	Color3f FragmentFunc(const Vector2f& uv, float ratio)
	{
		Vector2f ratioUV = (uv * 2.0f - 1.0f) * Vector2f(ratio, 1.0f);
		
		// Create ray
		Ray ray;

		Matrix3x3 abc = Matrix3x3::Identity;
		{
			
			const float* tt = matrixRotation.get();
			abc.c0[0] = tt[0]; abc.c0[1] = tt[1]; abc.c0[2] = tt[2];
			abc.c1[0] = tt[4]; abc.c1[1] = tt[5]; abc.c1[2] = tt[6];
			abc.c2[0] = tt[8]; abc.c2[1] = tt[9]; abc.c2[2] = tt[10];
			_camera.m_Rotation = Quaternion::MatrixToQuaternion(abc);
			int a = 3;
		}

		ray.m_Origin = _camera.m_Position;
		ray.m_Direction = Vector3f::Normalize(Quaternion::RotateVector(_camera.m_Rotation, Vector3f(-ratioUV.x, ratioUV.y, -1.0f)));
		
		Vector3 temp2 = matrixRotation * Vector3(-ratioUV.x, ratioUV.y, -1);
		temp2.normalize();

		if (fabs(temp2.x - ray.m_Direction.x) > 1e-3 || fabs(temp2.z - ray.m_Direction.z) > 1e-3 || fabs(temp2.y - ray.m_Direction.y) > 1e-3)
		{
			int a = 3;
		}
		//ray.m_Direction = Vector3f(temp2.x, temp2.y, temp2.z);

		
		// Raymarching for hit point
		float raymarchingDistance = Raymarching(ray);

		// Final color shading
		Color3f surfaceColor = Color3f::Black;
		if (raymarchingDistance < kRaymarchDistanceMax)
		{
			Vector3f surfacePosition = ray.m_Origin + ray.m_Direction * raymarchingDistance;
			Vector3f surfaceNormal = GetSurfaceNormal(surfacePosition);
			surfaceColor = Lighting(surfacePosition, surfaceNormal);
		}

		return surfaceColor;
	}

	float Raymarching(const Ray& ray)
	{
		float raymarchingDistance = 0.0f;
		for (int i = 0; i < kRaymarchStepMax; ++i)
		{
			Vector3f raymarchingPosition = ray.m_Origin + ray.m_Direction * raymarchingDistance;
			float distanceToNearestSurface = _scene.GetMinDistance(raymarchingPosition);
			if (distanceToNearestSurface < kRaymarchDistanceMin)
				break;

			raymarchingDistance += distanceToNearestSurface;
			if (raymarchingDistance > kRaymarchDistanceMax)
				break;
		}

		return raymarchingDistance;
	}

	float Shadowing(const Vector3f& position)
	{
		// Shadow Ray
		Ray rayShadow;
		rayShadow.m_Origin = position;
		rayShadow.m_Direction = _light.m_Direction;

		// Ray marching for shadow
		float shadowDistanceToNearestSurface = Raymarching(rayShadow);
		if (shadowDistanceToNearestSurface < kRaymarchDistanceMax)
			return 0.0f;

		return 1.0f;
	}


	Vector3f GetSurfaceNormal(const Vector3f& surfacePosition)
	{
		float distanceToNearestSurface = _scene.GetMinDistance(surfacePosition);
		Vector3f normalDirection = Vector3f
		(
			distanceToNearestSurface - _scene.GetMinDistance(surfacePosition - Vector3f(0.01f, 0.00f, 0.00f)),
			distanceToNearestSurface - _scene.GetMinDistance(surfacePosition - Vector3f(0.00f, 0.01f, 0.00f)),
			distanceToNearestSurface - _scene.GetMinDistance(surfacePosition - Vector3f(0.00f, 0.00f, 0.01f))
		);

		return Vector3f::Normalize(normalDirection);
	}

	Color3f Lighting(const Vector3f& position, const Vector3f& normalDirection)
	{
		// Lambert
		float NdotL = Vector3f::Dot(_light.m_Direction, normalDirection);
		float lambert = MathUtility::Saturate(NdotL);

		// Shadow
		float shadow = Shadowing(position + normalDirection * kRaymarchDistanceMin * 2.0f);

		// Output
		float lightingIntensity = MathUtility::Lerp(kLightGI, 1.0f, lambert * shadow);
		Color3f outputColor = kRaymarchSurfaceColor * _light.m_Color * lightingIntensity;
		return outputColor;
	}

private:

	Scene _scene;
	Light _light;
	Camera _camera;

	SDFBox _box;
	SDFSphere _sphere;
	SDFSubtract _boxSubtractSphere;

	SDFPlane _plane;

private:

	const float    kLightGI = 0.3f;
	const Color3f  kLightColor = Color3f::White;
	const Vector3f kLightPosition = Vector3f(20.0f, 20.0f, -5.0f);

	const Vector3f kCameraPosition = Vector3f(0.0f, 1.5f, -4.0f);

	const float      kSphereRadius = 1.0f;
	const Vector3f   kSpherePosition = Vector3f(0.0f, 0.0f, 0.0f);
	const Quaternion kSphereRotation = Quaternion(Vector3f::Normalize(kCameraPosition), 0); ////Quaternion::Identity;
	const Vector3f   kBoxPosition = Vector3f(0.0f, 0.0f, 0.0f);
	const Vector3f   kBoxSize = Vector3f(0.8f, 0.8f, 0.8f);
	const Quaternion kBoxRotation = Quaternion(Vector3f::Normalize(kCameraPosition), 0); //Quaternion::Identity;
	const Quaternion kBoxSphereRotation = Quaternion(Vector3f::Normalize(kCameraPosition),0); //Quaternion::AxisRadian(Vector3f::Up, MathUtility::PI * 0.15f);

	const Vector3f   kPlanePosition = Vector3f(0.0f, -0.8f, 0.0f);
	const Vector3f   kPlaneNormal = Vector3f(0.0f, 1.0f, 0.0f);
	const Quaternion kPlaneRotation = Quaternion::Identity;

	const int     kRaymarchStepMax = 100;
	const float   kRaymarchDistanceMax = 100.0f;
	const float   kRaymarchDistanceMin = 0.0001f;
	const Color3f kRaymarchSurfaceColor = Color3f(0.5f, 0.8f, 1.0f);
};