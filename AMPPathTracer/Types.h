#pragma once

#include "Vector3.h"

#include <json-cpp.hpp>

struct Intersection;

struct Material
{
	Material() restrict(amp, cpu)
	{
		Emission = { 0.0f, 0.0f, 0.0f };
		DiffuseColor = { 0.0f, 0.0f, 0.0f };
		SpecularColor = { 1.0f, 1.0f, 1.0f };
		Reflectivity = 0.0f;
		Transmittance = 0.0f;
		RefractiveIndex = 1.0f;
		Roughness = 0.0f;
	}

	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(Emission),
			JSON_NVP(DiffuseColor),
			JSON_NVP(SpecularColor),
			JSON_NVP(Reflectivity),
			JSON_NVP(Transmittance),
			JSON_NVP(RefractiveIndex),
			JSON_NVP(Roughness)
			);
	}

	Vector3 Emission;
	Vector3 DiffuseColor;
	Vector3 SpecularColor;
	float Reflectivity;
	float Transmittance;
	float RefractiveIndex;
	float Roughness;
};

struct Ray
{
	Vector3 Origin;
	Vector3 Direction;
};

struct SphereData
{
	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(Origin),
			JSON_NVP(Radius)
			);
	}

	Vector3 Origin;
	float Radius;
};

struct PlaneData
{
	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(Point),
			JSON_NVP(Normal)
			);
	}

	Vector3 Point;
	Vector3 Normal;
};

enum ObjectType
{
	OBJECT_SPHERE,
	OBJECT_PLANE,
};

class SceneObject
{
public:
	SceneObject() restrict(amp, cpu)
	{
	}

	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(Type),
			JSON_NVP(Sphere),
			JSON_NVP(Plane),
			JSON_NVP(Properties)
			);
	}

	// This is an int to stop json breaking
	int Type;

	union
	{
		SphereData Sphere;
		PlaneData Plane;
	};

	Material Properties;

	Intersection Trace(Ray r) restrict(amp, cpu);

	Intersection TraceSphere(Ray r) restrict(amp, cpu);
	Intersection TracePlane(Ray r) restrict(amp, cpu);
};

struct Intersection
{
	Intersection(Vector3 position, Vector3 normal, SceneObject object) restrict(amp, cpu)
	{
		Position = position;
		Normal = normal;
		Object = object;
		Valid = true;
	}

	Intersection() restrict(amp, cpu)
	{
		Valid = false;
	}

	Vector3 Position;
	Vector3 Normal;

	SceneObject Object;

	bool Valid;
};

struct Coord
{
	int X;
	int Y;
};