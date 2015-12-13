#pragma once

#include "LCGRandom.h"

#include <vector>
#include <amp.h>

using namespace concurrency;

#include "Types.h"

class Scene
{
public:
    static V3 TraceRay(Ray r, LCGRandom *randomGen, int bounces, array<SceneObject, 1> &objects) restrict(amp);
    static V3 TraceRayCPU(Ray r, LCGRandom *randomGen, int bounces, std::vector<SceneObject> objects) restrict(cpu);
};