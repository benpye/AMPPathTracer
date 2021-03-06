#pragma once

#include "Vector3.h"
#include "LCGRandom.h"
#include "Types.h"

class Camera
{
public:
	Camera() restrict(amp, cpu);

	void Move(Vector3 direction) restrict(amp, cpu);

	void ChangeDirection(float dpitch, float dyaw) restrict(amp, cpu);

	void CalculateDirection() restrict(amp, cpu);

	void CalculateScreenDistance(int width) restrict(amp, cpu);

	Vector3 CalculateDirectionForPixel(float x, float y, int width, int height) const restrict(amp, cpu);

	Ray CalculateRayForPixelDoF(float x, float y, int width, int height, LCGRandom *random) const restrict(amp, cpu);

	Vector3 Forward;
	Vector3 Right;
	Vector3 Up;

	Vector3 Origin;

	float Pitch;
	float Yaw;
	float Fov;
	float ScreenDistance;
	float FocalPoint;
	float Aperture;
};