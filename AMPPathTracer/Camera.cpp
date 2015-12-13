#include "Common.h"

#include "Camera.h"

#include <amp_math.h>
using namespace concurrency::fast_math;

Camera::Camera() restrict(amp, cpu)
{
    Pitch = 0.0f;
    Yaw = 0.0f;

    CalculateDirection();
}

void Camera::Move(V3 direction) restrict(amp, cpu)
{
    Origin += direction;
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

    Right = Forward.Cross({ 0.0f, 0.0f, 1.0f }).Normalize();
    Up = -Forward.Cross(Right).Normalize();
}

void Camera::CalculateScreenDistance(int width) restrict(amp, cpu)
{
    ScreenDistance = (((float)width) / 2.0f) / tanf(Fov / 2.0f);
}

V3 Camera::CalculateDirectionForPixel(float x, float y, int width, int height) const restrict(amp, cpu)
{
    return ((Forward * ScreenDistance)
        + (-Up * (y - ((float)height / 2.0f)))
        + (Right * (x - ((float)width / 2.0f)))).Normalize();
}