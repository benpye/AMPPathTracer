#pragma once

#include <cmath>
#include <amp_math.h>
#include <string>
#include <sstream>

#include <json-cpp.hpp>

using namespace concurrency::fast_math;

class Vector3
{
public:
	Vector3() restrict(amp, cpu)
	{
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
	}

	Vector3(float s) restrict(amp, cpu)
	{
		X = s;
		Y = s;
		Z = s;
	}

	Vector3(float _x, float _y, float _z) restrict(amp, cpu)
	{
		X = _x;
		Y = _y;
		Z = _z;
	}

	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(X),
			JSON_NVP(Y),
			JSON_NVP(Z)
			);
	}

	// Operators with two vectors
	Vector3 operator+(const Vector3& v) const restrict(amp, cpu)
	{
		return Vector3(X + v.X, Y + v.Y, Z + v.Z);
	}

	Vector3 operator-(const Vector3& v) const restrict(amp, cpu)
	{
		return Vector3(X - v.X, Y - v.Y, Z - v.Z);
	}

	// These are not mathmaticallY correct, theY operate per component
	Vector3 operator*(const Vector3& v) const restrict(amp, cpu)
	{
		return Vector3(X * v.X, Y * v.Y, Z * v.Z);
	}

	Vector3 operator/(const Vector3& v) const restrict(amp, cpu)
	{
		return Vector3(X / v.X, Y / v.Y, Z / v.Z);
	}

	// Scalar operations
	Vector3 operator*(float d) const restrict(amp, cpu)
	{
		return *this * Vector3(d);
	}

	Vector3 operator/(float d) const restrict(amp, cpu)
	{
		return *this / Vector3(d);
	}

	// UnarY minus operator
	Vector3 operator-() const restrict(amp, cpu)
	{
		return Vector3(-X, -Y, -Z);
	}

	// Equality operator
	bool operator==(const Vector3& v) const restrict(amp, cpu)
	{
		return ((X == v.X) && (Y == v.Y) && (Z == v.Z));
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

	float X;
	float Y;
	float Z;
};