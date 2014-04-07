#pragma once

#include <cmath>
#include <amp_math.h>
#include <string>
#include <sstream>

using namespace concurrency::fast_math;

#define GRAPHICS_VEC

#ifdef GRAPHICS_VEC
#include <amp_graphics.h>
using namespace concurrency::graphics;
#endif

class Vector3
{
public:
	Vector3() restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value = float_3(0.0f);
#else
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
#endif
	}

	Vector3(float s) restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value = float_3(s);
#else
		X = s;
		Y = s;
		Z = s;
#endif
	}

	Vector3(float _x, float _y, float _z) restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value = float_3(_x, _y, _z);
#else
		X = _x;
		Y = _y;
		Z = _z;
#endif
	}

#ifdef GRAPHICS_VEC
	Vector3(float_3 v) restrict(amp, cpu)
	{
		value = v;
	}
#endif

	// Operators with two vectors
	Vector3 operator+(const Vector3& v) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value + v.value);
#else
		return Vector3(X + v.X, Y + v.Y, Z + v.Z);
#endif
	}

	Vector3 operator-(const Vector3& v) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value - v.value);
#else
		return Vector3(X + v.X, Y + v.Y, Z + v.Z);
#endif
	}

	// These are not mathmaticallY correct, theY operate per component
	Vector3 operator*(const Vector3& v) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value * v.value);
#else
		return Vector3(X * v.X, Y * v.Y, Z * v.Z);
#endif
	}

	Vector3 operator/(const Vector3& v) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value / v.value);
#else
		return Vector3(X / v.X, Y / v.Y, Z / v.Z);
#endif
	}

	// Scalar operations
	Vector3 operator*(float d) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value * d);
#else
		return *this * Vector3(d);
#endif
	}

	Vector3 operator/(float d) const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(value / d);
#else
		return *this / Vector3(d);
#endif
	}

	// UnarY minus operator
	Vector3 operator-() const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return Vector3(-value);
#else
		return Vector3(-X, -Y, -Z);
#endif
	}

	// More compleX math operations
	float Dot(const Vector3& v) const restrict(amp, cpu)
	{
		return X * v.X + Y * v.Y + Z * v.Z;
	}

	Vector3 Cross(const Vector3& v) const restrict(amp, cpu)
	{
		return Vector3(Y * v.Z - Z * v.Y, Z * v.X - X * v.Z, X * v.Y - Y * v.X);
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

	Vector3 Normalize() const restrict(amp, cpu)
	{
		return *this / Length();
	}

	Vector3 Max(float d) const restrict(amp, cpu)
	{
		return Vector3(fmaxf(X, d), fmaxf(Y, d), fmaxf(Z, d));
	}

	Vector3 Min(float d) const restrict(amp, cpu)
	{
		return Vector3(fminf(X, d), fminf(Y, d), fminf(Z, d));
	}

	Vector3 Pow(float d) const restrict(amp, cpu)
	{
		return Vector3(powf(X, d), powf(Y, d), powf(Z, d));
	}

	// Path tracing specific functions
	Vector3 Reflect(const Vector3& normal) const restrict(amp, cpu)
	{
		return *this - normal * 2.0 * Dot(normal);
	}

	Vector3 Refract(const Vector3& normal, float eta) const restrict(amp, cpu)
	{
		float cosi = (-*this).Dot(normal);
		float cost2 = 1.0f - eta * eta * (1.0f - cosi * cosi);
		Vector3 t = (*this) * eta + (normal * (eta*cosi - sqrtf(fabs(cost2))));
		if (cost2 > 0.0f)
			return t;
		else
			return Vector3();
	}

	std::string ToString() const
	{
		std::stringstream temp;
		temp << "(" << X << ", " << Y << ", " << Z << ")";
		return temp.str();
	}

	float GetX() const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return value.x;
#else
		return _x;
#endif
	}

	float GetY() const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return value.y;
#else
		return _y;
#endif
	}

	float GetZ() const restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		return value.z;
#else
		return _z;
#endif
	}

	void SetX(float v) restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value.x = v;
#else
		_x = v;
#endif
	}

	void SetY(float v) restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value.y = v;
#else
		_y = v;
#endif
	}

	void SetZ(float v) restrict(amp, cpu)
	{
#ifdef GRAPHICS_VEC
		value.z = v;
#else
		_z = v;
#endif
	}

	__declspec(property(get = GetX, put = SetX))
	float X;

	__declspec(property(get = GetY, put = SetY))
	float Y;

	__declspec(property(get = GetZ, put = SetZ))
	float Z;

private:
#ifdef GRAPHICS_VEC
	float_3 value;
#else
	float _x;
	float _y;
	float _z;
#endif
};