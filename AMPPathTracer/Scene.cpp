#include "Common.h"

#include "Scene.h"
#include "LCGRandom.h"

#include <float.h>

V3 CosineWeightedInCone(const V3& normal, LCGRandom *randomGen) restrict(amp, cpu)
{
    float Xi1 = randomGen->NextFloat();
    float Xi2 = randomGen->NextFloat();

    float theta = acosf(sqrtf(Xi1));
    float phi = 2.0f * M_PI * Xi2;

    return normal.RotateByAngles(theta, phi);
}

V3 UniformWeightedInCone(const V3& normal, LCGRandom *randomGen) restrict(amp, cpu)
{
    float Xi1 = randomGen->NextFloat();
    float Xi2 = randomGen->NextFloat();

    float theta = Xi1 * M_PI_OVER_2;
    float phi = 2.0f * M_PI * Xi2;

    return normal.RotateByAngles(theta, phi);
}

V3 Scene::TraceRayCPU(Ray r, LCGRandom *randomGen, int bounces, std::vector<SceneObject> objects) restrict(cpu)
{
    V3 colorAcc = { 0.0f, 0.0f, 0.0f };
    V3 reflectanceAcc = { 1.0f, 1.0f, 1.0f };

    for (int bounce = 0; bounce < bounces; bounce++)
    {
        V3 color;

        // Get nearest intersection
        Intersection nearestI;
        float lengthSqr = FLT_MAX;

        for (unsigned int i = 0; i < objects.size(); i++)
        {
            SceneObject obj = objects[i];
            Intersection tempI = obj.Trace(r);

            if (!tempI.Valid)
                continue;

            float lengthNew = (tempI.Position - r.Origin).LengthSquared();

            if (lengthNew < lengthSqr)
            {
                nearestI = tempI;
                lengthSqr = lengthNew;
            }
        }

        if (nearestI.Valid)
        {
            V3 dir;

            color = { 1.0f, 1.0f, 1.0f };

            V3 normal = nearestI.Normal;

            V3 normal2 = nearestI.Normal.Dot(r.Direction) < 0.0f ? normal : -normal;

            bool inside = normal.Dot(normal2) < 0.0f;

            float reflectivity = nearestI.Object.Properties.Reflectivity;

            float rValue = randomGen->NextFloat();

            if (rValue < reflectivity)
            {
                dir = r.Direction.Reflect(normal);

                // Specular color to approximate metals that tint reflections
                color = nearestI.Object.Properties.SpecularColor;
            }
            else
            {
                dir = CosineWeightedInCone(nearestI.Normal, randomGen);

                // Diffuse reflection affected by surface color
                color = nearestI.Object.Properties.DiffuseColor;
            }

            colorAcc += reflectanceAcc * nearestI.Object.Properties.Emission;
            reflectanceAcc *= color;

            r.Origin = nearestI.Position + dir * 1E-3f;
            r.Direction = dir;
        }
        else
        {
            break;
        }
    }

    return colorAcc;
}

V3 Scene::TraceRay(Ray r, LCGRandom *randomGen, int bounces, array<SceneObject> &objects) restrict(amp)
{
    V3 colorAcc = { 0.0f, 0.0f, 0.0f };
    V3 reflectanceAcc = { 1.0f, 1.0f, 1.0f };

    for (int bounce = 0; bounce < bounces; bounce++)
    {
        V3 color;

        // Get nearest intersection
        Intersection nearestI;
        float lengthSqr = FLT_MAX;

        for (unsigned int i = 0; i < objects.extent.size(); i++)
        {
            SceneObject obj = objects[index<1>(i)];
            Intersection tempI = obj.Trace(r);

            if (!tempI.Valid)
                continue;

            float lengthNew = (tempI.Position - r.Origin).LengthSquared();

            if (lengthNew < lengthSqr)
            {
                nearestI = tempI;
                lengthSqr = lengthNew;
            }
        }

        if (nearestI.Valid)
        {
            V3 dir;

            color = { 1.0f, 1.0f, 1.0f };

            V3 normal = nearestI.Normal;

            V3 normal2 = nearestI.Normal.Dot(r.Direction) < 0.0f ? normal : -normal;

            bool inside = normal.Dot(normal2) < 0.0f;

            float reflectivity = nearestI.Object.Properties.Reflectivity;

            float rValue = randomGen->NextFloat();

            if (rValue < reflectivity)
            {
                dir = r.Direction.Reflect(normal);

                // Specular color to approximate metals that tint reflections
                color = nearestI.Object.Properties.SpecularColor;
            }
            else
            {
                dir = CosineWeightedInCone(nearestI.Normal, randomGen);

                // Diffuse reflection affected by surface color
                color = nearestI.Object.Properties.DiffuseColor;
            }

            colorAcc += reflectanceAcc * nearestI.Object.Properties.Emission;
            reflectanceAcc *= color;

            r.Origin = nearestI.Position + dir * 1E-3f;
            r.Direction = dir;
        }
        else
        {
            break;
        }
    }

    return colorAcc;
}