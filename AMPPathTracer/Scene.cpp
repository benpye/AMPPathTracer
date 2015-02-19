#include "Common.h"

#include "Scene.h"
#include "LCGRandom.h"

#include <float.h>

Vector3 CosineWeightedInCone(const Vector3& normal, float maxAngleOverPi, LCGRandom *randomGen) restrict(amp, cpu)
{
	float Xi1 = randomGen->NextFloat();
	float Xi2 = randomGen->NextFloat();

	float theta = acosf(sqrtf(Xi1)) * maxAngleOverPi;
	float phi = 2.0f * M_PI * Xi2;

	return normal.RotateByAngles(theta, phi);
}

Vector3 UniformWeightedInCone(const Vector3& normal, float maxAngleOverPi, LCGRandom *randomGen) restrict(amp, cpu)
{
	float Xi1 = randomGen->NextFloat();
	float Xi2 = randomGen->NextFloat();

	float theta = Xi1 * M_PI_OVER_2 * maxAngleOverPi;
	float phi = 2.0f * M_PI * Xi2;

	return normal.RotateByAngles(theta, phi);
}

float CalculateFresnel(Vector3 direction, Vector3 normal, float lastri, float thisri, float N) restrict(amp, cpu)
{
	float cosAoi = direction.Dot(-normal);

	float n1cosAoi = lastri * cosAoi;
	float n2cosAoi = thisri * cosAoi;

	float sinAoi = sin(acos(cosAoi));

	float rsqrt = sqrtf(1 - N * sinAoi * N * sinAoi);

	float rssqrt = thisri * rsqrt;
	float rs = (n1cosAoi - rssqrt) / (n1cosAoi + rssqrt);
	rs = rs * rs;

	float rpsqrt = lastri * rsqrt;
	float rp = (rpsqrt - n2cosAoi) / (rpsqrt + n2cosAoi);
	rp = rp * rp;

	return (rs + rp) / 2;
}

Vector3 Scene::TraceRay(Ray r, LCGRandom *randomGen, int bounces, float threshold, Vector3 ambientEmission, array<SceneObject> &objects) restrict(amp, cpu)
{
	Vector3 colorAcc = { 0.0f, 0.0f, 0.0f };
	Vector3 reflectanceAcc = { 1.0f, 1.0f, 1.0f };

	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Vector3 color;

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
			Vector3 dir;

			color = { 1.0f, 1.0f, 1.0f };

			float thisri = nearestI.Object.Properties.RefractiveIndex;
			float lastri = 1.0f;

			Vector3 normal = CosineWeightedInCone(nearestI.Normal, nearestI.Object.Properties.Roughness, randomGen);

			Vector3 normal2 = nearestI.Normal.Dot(r.Direction) < 0.0f ? normal : -normal;

			bool inside = normal.Dot(normal2) < 0.0f;

			if (inside)
			{
				lastri = thisri;
				thisri = 1.0f;
			}

			float N = lastri / thisri;
			float fresnel = CalculateFresnel(r.Direction, normal2, lastri, thisri, N);

			float reflectivity = nearestI.Object.Properties.Reflectivity;
			float transmittance = nearestI.Object.Properties.Transmittance;

			float rValue = randomGen->NextFloat();

			if (rValue < reflectivity * fresnel)
			{
				dir = r.Direction.Reflect(normal);

				// Specular color to approximate metals that tint reflections
				color = nearestI.Object.Properties.SpecularColor;
			}
			else if (rValue < transmittance * (1.0f - fresnel))
			{
				dir = r.Direction.Refract(normal2, N);

				if (dir == Vector3{ 0.0f, 0.0f, 0.0f })
				{
					dir = r.Direction.Reflect(normal); // Total internal reflection

					color = nearestI.Object.Properties.SpecularColor;
				}

				if (inside)
				{
					//float absorbivity = nearestI.Object.Properties.Absorbivity;

					//float dist = (nearestI.Position - r.Origin).Length();

					// TODO: Accurate beer's law

					color *= nearestI.Object.Properties.DiffuseColor;
				}
			}
			else
			{
				dir = CosineWeightedInCone(nearestI.Normal, 1.0f, randomGen);

				// Diffuse reflection affected by surface color
				color = nearestI.Object.Properties.DiffuseColor;
			}

			colorAcc += reflectanceAcc * nearestI.Object.Properties.Emission;
			reflectanceAcc *= color;

			r.Origin = nearestI.Position +dir *1E-3f;
			r.Direction = dir;
		}
		else
		{
			colorAcc += reflectanceAcc * ambientEmission;
			break;
		}

		if ((reflectanceAcc.X <= threshold) && (reflectanceAcc.Y <= threshold) && (reflectanceAcc.Z <= threshold))
			return colorAcc;
	}

	return colorAcc;
}