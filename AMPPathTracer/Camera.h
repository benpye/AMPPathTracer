#pragma once

#include "Vector3.h"
#include "LCGRandom.h"
#include "Types.h"

class Camera
{
public:
    Camera() restrict(amp, cpu);

    void Move(V3 direction) restrict(amp, cpu);

    void ChangeDirection(float dpitch, float dyaw) restrict(amp, cpu);

    void CalculateDirection() restrict(amp, cpu);

    void CalculateScreenDistance(int width) restrict(amp, cpu);

    V3 CalculateDirectionForPixel(float x, float y, int width, int height) const restrict(amp, cpu);

    V3 Forward;
    V3 Right;
    V3 Up;

    V3 Origin;

    float Pitch;
    float Yaw;
    float Fov;
    float ScreenDistance;
};