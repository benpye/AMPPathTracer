#pragma once

#include "LCGRandom.h"

#include <vector>
#include <amp.h>

using namespace concurrency;

#include "Types.h"

#define SCENE_SIZE 9

class Scene
{
public:
	Scene() restrict(amp, cpu);

	Vector3 TraceRay(Ray r, LCGRandom *randomGen, int bounces, float threshold) const restrict(amp, cpu);

	SceneObject Objects[SCENE_SIZE];

	Material Sky;
};