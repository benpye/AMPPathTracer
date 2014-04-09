#pragma once

#include "Vector3.h"
#include "LCGRandom.h"
#include "Scene.h"
#include "Camera.h"
#include "Controller.h"

#include <amp.h>
#include <amp_graphics.h>

using namespace concurrency;
using namespace graphics;

class PathTracer
{
public:
	PathTracer(int _width, int _height);

	void InitBuffers();
	void LoadScene(std::vector<SceneObject> objects);
	void RenderIteration();
	void GetPixelBuffer(int *buffer);
	int GetIteration();
	void Reset();

	~PathTracer();

	Camera Cam;
	int Recursions;
	float Threshold;
	Vector3 AmbientEmission;

private:
	int width;
	int height;
	int iteration;

	texture<int, 2> pixelBuffer;
	array<Vector3, 1> accumulationBuffer;
	array<LCGRandom, 1> rngBuffer;
	array<Coord, 1> coordLookupBuffer;

	array<SceneObject, 1> *sceneBuffer;

	extent<1> bufferSize;
};

