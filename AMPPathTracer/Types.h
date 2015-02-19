#pragma once

#include "Vector3.h"

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
	Vector3 Origin;
	float Radius;
};

struct PlaneData
{
	Vector3 Point;
	Vector3 Normal;
};

enum ObjectType
{
	OBJECT_SPHERE,
	OBJECT_PLANE,
	OBJECT_INVALID
};

class SceneObject
{
public:
	SceneObject() restrict(amp, cpu)
	{
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