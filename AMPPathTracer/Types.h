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
    }

    V3 Emission;
    V3 DiffuseColor;
    V3 SpecularColor;
    float Reflectivity;
};

struct Ray
{
    V3 Origin;
    V3 Direction;
};

struct SphereData
{
    V3 Origin;
    float Radius;
};

struct PlaneData
{
    V3 Point;
    V3 Normal;
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
    Intersection(V3 position, V3 normal, SceneObject object) restrict(amp, cpu)
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

    V3 Position;
    V3 Normal;

    SceneObject Object;

    bool Valid;
};

struct Coord
{
    int X;
    int Y;
};