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
		return{ X + v.X, Y + v.Y, Z + v.Z };
	}

	Vector3 operator-(const Vector3& v) const restrict(amp, cpu)
	{
		return{ X - v.X, Y - v.Y, Z - v.Z };
	}

	// These are not mathmaticallY correct, theY operate per component
	Vector3 operator*(const Vector3& v) const restrict(amp, cpu)
	{
		return{ X * v.X, Y * v.Y, Z * v.Z };
	}

	Vector3 operator/(const Vector3& v) const restrict(amp, cpu)
	{
		return{ X / v.X, Y / v.Y, Z / v.Z };
	}

	// Scalar operations
	Vector3 operator*(float d) const restrict(amp, cpu)
	{
		return *this * Vector3{ d, d, d };
	}

	Vector3 operator/(float d) const restrict(amp, cpu)
	{
		return *this / Vector3{ d, d, d };
	}

	// UnarY minus operator
	Vector3 operator-() const restrict(amp, cpu)
	{
		return{ -X, -Y, -Z };
	}

	// Equality operator
	bool operator==(const Vector3& v) const restrict(amp, cpu)
	{
		return ((X == v.X) && (Y == v.Y) && (Z == v.Z));
	}

	// Assignment operators
	Vector3 operator+=(const Vector3& v) restrict(amp, cpu)
	{
		X += v.X; Y += v.Y; Z += v.Z;
		return *this;
	}

	Vector3 operator-=(const Vector3& v) restrict(amp, cpu)
	{
		X -= v.X; Y -= v.Y; Z -= v.Z;
		return *this;
	}

	Vector3 operator*=(const Vector3& v) restrict(amp, cpu)
	{
		X *= v.X; Y *= v.Y; Z *= v.Z;
		return *this;
	}

	Vector3 operator/=(const Vector3& v) restrict(amp, cpu)
	{
		X /= v.X; Y /= v.Y; Z /= v.Z;
		return *this;
	}

	// More compleX math operations
	float Dot(const Vector3& v) const restrict(amp, cpu)
	{
		return X * v.X + Y * v.Y + Z * v.Z;
	}

	Vector3 Cross(const Vector3& v) const restrict(amp, cpu)
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

	Vector3 Normalize() const restrict(amp, cpu)
	{
		return *this / Length();
	}

	Vector3 Max(float d) const restrict(amp, cpu)
	{
		return{ fmaxf(X, d), fmaxf(Y, d), fmaxf(Z, d) };
	}

	Vector3 Min(float d) const restrict(amp, cpu)
	{
		return{ fminf(X, d), fminf(Y, d), fminf(Z, d) };
	}

	Vector3 Pow(float d) const restrict(amp, cpu)
	{
		return{ powf(X, d), powf(Y, d), powf(Z, d) };
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
			return{ 0.0f, 0.0f, 0.0f };
	}

	// Find vector by rotation
	Vector3 RotateByAngles(float theta, float phi) const restrict(amp, cpu)
	{
		float xs = sinf(theta) * cosf(phi);
		float ys = cosf(theta);
		float zs = sinf(theta) * sinf(phi);

		Vector3 h = *this;
		if (fabs(h.X) <= fabs(h.Y) && fabs(h.X) <= fabs(h.Z))
			h.X = 1.0f;
		else if (fabs(h.Y) <= fabs(h.X) && fabs(h.Y) <= fabs(h.Z))
			h.Y = 1.0f;
		else
			h.Z = 1.0f;


		Vector3 x = Cross(h).Normalize();
		Vector3 z = Cross(x).Normalize();

		Vector3 direction = x * xs + *this * ys + z * zs;
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