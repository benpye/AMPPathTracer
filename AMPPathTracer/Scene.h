#pragma once

#include "LCGRandom.h"

#include <vector>
#include <amp.h>

using namespace concurrency;

#include "Types.h"

class Scene
{
public:
	static Vector3 TraceRay(Ray r, LCGRandom *randomGen, int bounces, float threshold, Vector3 ambientEmission, array<SceneObject, 1> &objects) restrict(amp, cpu);
};