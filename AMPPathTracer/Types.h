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
		Reflectivity = 0.0;
		Transmittance = 0.0;
		RefractiveIndex = 1.0;
		Absorbivity = 0.0;
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
			JSON_NVP(Absorbivity)
			);
	}

	Vector3 Emission;
	Vector3 DiffuseColor;
	Vector3 SpecularColor;
	float Reflectivity;
	float Transmittance;
	float RefractiveIndex;
	float Absorbivity;
};

struct Ray
{
	Vector3 Origin;
	Vector3 Direction;
};

struct SphereData
{
	Vector3 Origin;
	float Radius;
};

template<typename T>
inline void serialize(jsoncpp::Stream<T>& stream, SphereData& o)
{
	fields(o, stream,
		"Origin", o.Origin,
		"Radius", o.Radius
		);
}

struct PlaneData
{
	Vector3 Point;
	Vector3 Normal;
};

template<typename T>
inline void serialize(jsoncpp::Stream<T>& stream, PlaneData& o)
{
	fields(o, stream,
		"Point", o.Point,
		"Normal", o.Normal
		);
}

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

	bool Valid;

	SceneObject Object;
};

struct Coord
{
	int X;
	int Y;
};