#include "Scene.h"
#include "LCGRandom.h"

#include <float.h>

Vector3 CosineWeightedOnHemisphere(const Vector3& normal, LCGRandom *randomGen) restrict(amp, cpu)
{
	float Xi1 = randomGen->NextFloat();
	float Xi2 = randomGen->NextFloat();

	float theta = acosf(sqrtf(Xi1));
	float phi = 2.0f * 3.1415926535897932384626433832795f * Xi2;

	float xs = sinf(theta) * cosf(phi);
	float ys = cosf(theta);
	float zs = sinf(theta) * sinf(phi);

	Vector3 h = normal;
	if (fabs(h.X) <= fabs(h.Y) && fabs(h.X) <= fabs(h.Z))
		h.X = 1.0f;
	else if (fabs(h.Y) <= fabs(h.X) && fabs(h.Y) <= fabs(h.Z))
		h.Y = 1.0f;
	else
		h.Z = 1.0f;


	Vector3 x = h.Cross(normal).Normalize();
	Vector3 z = x.Cross(normal).Normalize();

	Vector3 direction = x * xs + normal * ys + z * zs;
	return direction.Normalize();
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

Scene::Scene() restrict(amp, cpu)
{

}

Vector3 Scene::TraceRay(Ray r, LCGRandom *randomGen, int bounces, float threshold) const restrict(amp, cpu)
{
	Vector3 colorAcc = Vector3();
	Vector3 reflectanceAcc = Vector3(1.0f);

	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Vector3 color;

		// Get nearest intersection
		Intersection nearestI;
		float lengthSqr = FLT_MAX;

		for (int i = 0; i < SCENE_SIZE; i++)
		{
			SceneObject obj = Objects[i];
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

			color = Vector3(1.0f);

			float thisri = nearestI.Object.Properties.RefractiveIndex;
			float lastri = 1.0f;

			Vector3 normal2 = nearestI.Normal.Dot(r.Direction) < 0.0f ? nearestI.Normal : -nearestI.Normal;

			bool inside = nearestI.Normal.Dot(normal2) < 0.0f;

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
				dir = r.Direction.Reflect(nearestI.Normal);

				// Specular color to approximate metals that tint reflections
				color = nearestI.Object.Properties.SpecularColor;
			}
			else if (rValue < transmittance * (1.0f - fresnel))
			{
				dir = r.Direction.Refract(normal2, N);

				if (dir.X == 0.0f && dir.Y == 0.0f && dir.Z == 0.0f)
				{
					dir = r.Direction.Reflect(nearestI.Normal); // Total internal reflection

					color = nearestI.Object.Properties.SpecularColor;
				}

				if (inside)
				{
					float absorbivity = nearestI.Object.Properties.Absorbivity;

					float dist = (nearestI.Position - r.Origin).Length();

					// TODO: Accurate beer's law

					color = color * nearestI.Object.Properties.DiffuseColor;
				}
			}
			else
			{
				dir = CosineWeightedOnHemisphere(nearestI.Normal, randomGen);

				// Diffuse reflection affected by surface color
				color = nearestI.Object.Properties.DiffuseColor;
			}

			colorAcc = colorAcc + reflectanceAcc * nearestI.Object.Properties.Emission;
			reflectanceAcc = reflectanceAcc * color;

			r.Origin = nearestI.Position + dir * 1E-3f;
			r.Direction = dir;
		}
		else
		{
			colorAcc = colorAcc + reflectanceAcc * Sky.Emission;
			break;
		}

		if ((reflectanceAcc.X <= threshold) && (reflectanceAcc.Y <= threshold) && (reflectanceAcc.Z <= threshold))
			return colorAcc;
	}

	return colorAcc;
}