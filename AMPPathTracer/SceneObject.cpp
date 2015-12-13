#include "Types.h"

#include <cmath>
#include <amp_math.h>

using namespace concurrency::fast_math;

Intersection SceneObject::Trace(Ray r) restrict(amp, cpu)
{
    switch (Type)
    {
    case OBJECT_SPHERE:
        return TraceSphere(r);
    case OBJECT_PLANE:
        return TracePlane(r);
    default:
        return Intersection();
    }
}

Intersection SceneObject::TraceSphere(Ray r) restrict(amp, cpu)
{
    // Based upon simplified code at http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
    // Get the ray origin relative to the sphere
    V3 rOrigin = r.Origin - Sphere.Origin;

    // Get A, B and C of quadratic
    float a = r.Direction.Dot(r.Direction);
    float b = 2 * rOrigin.Dot(r.Direction);
    float c = rOrigin.Dot(rOrigin) - (Sphere.Radius * Sphere.Radius);

    // Find discriminant
    float disc = b * b - 4 * a * c;

    if (disc < 0)
        return Intersection();

    float discSqrt = sqrtf(disc);

    // The two solutions, these may be the same
    float t0 = (-b - discSqrt) / (2 * a);
    float t1 = (-b + discSqrt) / (2 * a);

    // if t1 < 0 then the object lies behind the ray
    if (t1 < 0)
        return Intersection();

    V3 tpos;

    // Only return intersections that are on the positive side of the ray's origin
    // They must also be translated as they are currently relative to the sphere
    if (t0 >= 0)
    {
        tpos = r.Origin + r.Direction * t0;
        return Intersection(tpos, (tpos - Sphere.Origin).Normalize(), *this);
    }

    tpos = r.Origin + r.Direction * t1;
    return Intersection(tpos, (tpos - Sphere.Origin).Normalize(), *this);
}

Intersection SceneObject::TracePlane(Ray r) restrict(amp, cpu)
{
    // Based upon http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-plane-and-ray-disk-intersection/
    float denom = -r.Direction.Dot(Plane.Normal);

    if (denom > 0.0f)
    {
        V3 dir = r.Origin - Plane.Point;
        float t = dir.Dot(Plane.Normal) / denom;

        return Intersection(r.Origin + r.Direction * t, Plane.Normal, *this);
    }

    return Intersection();
}