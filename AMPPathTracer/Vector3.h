#pragma once

#include <cmath>
#include <amp_math.h>
#include <string>
#include <sstream>

using namespace concurrency::fast_math;

class V3
{
public:
    // Operators with two vectors
    V3 operator+(const V3& v) const restrict(amp, cpu)
    {
        return{ X + v.X, Y + v.Y, Z + v.Z };
    }

    V3 operator-(const V3& v) const restrict(amp, cpu)
    {
        return{ X - v.X, Y - v.Y, Z - v.Z };
    }

    // These are not mathmaticallY correct, theY operate per component
    V3 operator*(const V3& v) const restrict(amp, cpu)
    {
        return{ X * v.X, Y * v.Y, Z * v.Z };
    }

    V3 operator/(const V3& v) const restrict(amp, cpu)
    {
        return{ X / v.X, Y / v.Y, Z / v.Z };
    }

    // Scalar operations
    V3 operator*(float d) const restrict(amp, cpu)
    {
        return *this * V3{ d, d, d };
    }

    V3 operator/(float d) const restrict(amp, cpu)
    {
        return *this / V3{ d, d, d };
    }

    // UnarY minus operator
    V3 operator-() const restrict(amp, cpu)
    {
        return{ -X, -Y, -Z };
    }

    // Equality operator
    bool operator==(const V3& v) const restrict(amp, cpu)
    {
        return ((X == v.X) && (Y == v.Y) && (Z == v.Z));
    }

    // Assignment operators
    V3 operator+=(const V3& v) restrict(amp, cpu)
    {
        X += v.X; Y += v.Y; Z += v.Z;
        return *this;
    }

    V3 operator-=(const V3& v) restrict(amp, cpu)
    {
        X -= v.X; Y -= v.Y; Z -= v.Z;
        return *this;
    }

    V3 operator*=(const V3& v) restrict(amp, cpu)
    {
        X *= v.X; Y *= v.Y; Z *= v.Z;
        return *this;
    }

    V3 operator/=(const V3& v) restrict(amp, cpu)
    {
        X /= v.X; Y /= v.Y; Z /= v.Z;
        return *this;
    }

    // More compleX math operations
    float Dot(const V3& v) const restrict(amp, cpu)
    {
        return X * v.X + Y * v.Y + Z * v.Z;
    }

    V3 Cross(const V3& v) const restrict(amp, cpu)
    {
        return{ Y * v.Z - Z * v.Y, Z * v.X - X * v.Z, X * v.Y - Y * v.X };
    }

    // Used for faster sorting of intersects
    float LengthSquared() const restrict(amp, cpu)
    {
        return X * X + Y * Y + Z * Z;
    }

    float Length() const restrict(amp, cpu)
    {
        return sqrtf(LengthSquared());
    }

    V3 Normalize() const restrict(amp, cpu)
    {
        return *this / Length();
    }

    V3 Max(float d) const restrict(amp, cpu)
    {
        return{ fmaxf(X, d), fmaxf(Y, d), fmaxf(Z, d) };
    }

    V3 Min(float d) const restrict(amp, cpu)
    {
        return{ fminf(X, d), fminf(Y, d), fminf(Z, d) };
    }

    V3 Pow(float d) const restrict(amp, cpu)
    {
        return{ powf(X, d), powf(Y, d), powf(Z, d) };
    }

    // Path tracing specific functions
    V3 Reflect(const V3& normal) const restrict(amp, cpu)
    {
        return *this - normal * 2.0 * Dot(normal);
    }

    V3 Refract(const V3& normal, float eta) const restrict(amp, cpu)
    {
        float cosi = (-*this).Dot(normal);
        float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
        V3 t = (*this) * eta + (normal * (eta*cosi - sqrtf(fabs(cost2))));
        if (cost2 > 0.0f)
            return t;
        else
            return{ 0.0f, 0.0f, 0.0f };
    }

    // Find vector by rotation
    V3 RotateByAngles(float theta, float phi) const restrict(amp, cpu)
    {
        float xs = sinf(theta) * cosf(phi);
        float ys = cosf(theta);
        float zs = sinf(theta) * sinf(phi);

        V3 h = *this;
        if (fabs(h.X) <= fabs(h.Y) && fabs(h.X) <= fabs(h.Z))
            h.X = 1.0f;
        else if (fabs(h.Y) <= fabs(h.X) && fabs(h.Y) <= fabs(h.Z))
            h.Y = 1.0f;
        else
            h.Z = 1.0f;


        V3 x = Cross(h).Normalize();
        V3 z = Cross(x).Normalize();

        V3 direction = x * xs + *this * ys + z * zs;
        return direction.Normalize();
    }

    std::string ToString() const
    {
        std::stringstream temp;
        temp << "(" << X << ", " << Y << ", " << Z << ")";
        return temp.str();
    }

    float X;
    float Y;
    float Z;
};