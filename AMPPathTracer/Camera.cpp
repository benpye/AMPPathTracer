#include "Camera.h"

#include <amp_math.h>
using namespace concurrency::fast_math;

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

Camera::Camera() restrict(amp, cpu)
{
	Pitch = 0.0f;
	Yaw = 0.0f;

	CalculateDirection();
}

void Camera::Move(Vector3 direction) restrict(amp, cpu)
{
	Origin = Origin + direction;
}

void Camera::ChangeDirection(float dpitch, float dyaw) restrict(amp, cpu)
{
	Pitch += dpitch;
	Yaw += dyaw;
	CalculateDirection();
}

void Camera::CalculateDirection() restrict(amp, cpu)
{
	Forward.X = sinf(Yaw) * cosf(Pitch);
	Forward.Y = cosf(Yaw) * cosf(Pitch);
	Forward.Z = sinf(Pitch);

	Forward = Forward.Normalize();

	Right = Forward.Cross(Vector3(0.0f, 0.0f, 1.0f)).Normalize();
	Up = -Forward.Cross(Right).Normalize();
}

void Camera::CalculateScreenDistance(int width) restrict(amp, cpu)
{
	ScreenDistance = (((float) width) / 2.0f) / tanf(Fov / 2.0f);
}

Vector3 Camera::CalculateDirectionForPixel(float x, float y, int width, int height) const restrict(amp, cpu)
{
	return ((Forward * ScreenDistance)
		+ (-Up * (y - ((float) height / 2.0f)))
		+ (Right * (x - ((float) width / 2.0f)))).Normalize();
}

Ray Camera::CalculateRayForPixelDoF(float x, float y, int width, int height, LCGRandom *random) const restrict(amp, cpu)
{
	float theta = random->NextFloat() * 2.0f * M_PI;
	float r = sqrtf(random->NextFloat()) * Aperture;

	float rx = (r * cosf(theta)) - (Aperture / 2.0f);
	float ry = (r * sinf(theta)) - (Aperture / 2.0f);

	Vector3 point = Origin + CalculateDirectionForPixel(x, y, width, height) * FocalPoint;

	Vector3 jitOrigin = Origin +
		Right * rx +
		Up * ry;

	Vector3 dir = (point - jitOrigin).Normalize();

	return Ray(jitOrigin, dir);
}