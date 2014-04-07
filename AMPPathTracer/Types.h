#pragma once

#include "Vector3.h"

struct Intersection;

struct Material
{
	Material() restrict(amp, cpu)
	{
		Emission = Vector3();
		DiffuseColor = Vector3();
		SpecularColor = Vector3(1.0f);
		Reflectivity = 0.0;
		Transmittance = 0.0;
		RefractiveIndex = 1.0;
		Absorbivity = 0.0;
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
	Ray(Vector3 origin, Vector3 direction) restrict(amp, cpu)
	{
		Origin = origin;
		Direction = direction;
	}

	Vector3 Origin;
	Vector3 Direction;
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

	ObjectType Type;

	Vector3 Origin;

	// Plane value
	Vector3 Normal;

	// Sphere value
	float Radius;

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