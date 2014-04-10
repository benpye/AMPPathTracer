#include "Common.h"

#include "PathTracer.h"

#include <vector>

int ConstructColor(int r, int g, int b, int a) restrict(amp, cpu)
{
	return (r & 0xFF) + ((g & 0xFF) << 8) + ((b & 0xFF) << 16) + ((a & 0xFF) << 24);
}

PathTracer::PathTracer(int _width, int _height)
: pixelBuffer(_height, _width, 32U),
accumulationBuffer(_width * _height),
rngBuffer(_width * _height),
coordLookupBuffer(_width * _height),
bufferSize(_width * _height)
{
	width = _width;
	height = _height;

	iteration = 0;
}

PathTracer::~PathTracer()
{
	delete sceneBuffer;
}

void PathTracer::InitBuffers()
{
	srand((int) time(NULL));

	std::vector<LCGRandom> randomVector(width * height);
	std::generate(randomVector.begin(), randomVector.end(), [](){ return LCGRandom(rand());});
	copy(randomVector.begin(), randomVector.end(), rngBuffer);

	Coord *coordArray = new Coord[width * height];

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			coordArray[y * width + x] = { x, y };
		}
	}

	copy(coordArray, coordLookupBuffer);

	delete[] coordArray;
}

void PathTracer::LoadScene(std::vector<SceneObject> objects)
{
	sceneBuffer = new array<SceneObject, 1>(objects.size(), objects.begin(), objects.end());
}

void PathTracer::RenderIteration()
{
	iteration++;

	// We have to make local copies because C++AMP disallows access to class members (grr)
	texture<int, 2> _pixelBuffer = pixelBuffer;
	array<Vector3, 1> _accumulationBuffer = accumulationBuffer;
	array<LCGRandom, 1> _rngBuffer = rngBuffer;
	array<Coord, 1> _coordLookupBuffer = coordLookupBuffer;
	array<SceneObject, 1> _sceneBuffer = *sceneBuffer;

	Camera _cam = Cam;

	int _width = width;
	int _height = height;
	int _iteration = iteration;
	int _recursions = Recursions;
	float _threshold = Threshold;

	Vector3 _ambientEmission = AmbientEmission;

	parallel_for_each(
		bufferSize,
		[=, &_pixelBuffer, &_accumulationBuffer, &_rngBuffer, &_coordLookupBuffer, &_sceneBuffer](index<1> idx) restrict(amp)
	{
		Vector3 color;

		// Offset x/y by up to half a pixel in each direction to give free AA
		float x = _coordLookupBuffer[idx].X + _rngBuffer[idx].NextFloat() - 0.5f;
		float y = _coordLookupBuffer[idx].Y + _rngBuffer[idx].NextFloat() - 0.5f;

		Ray r = _cam.CalculateRayForPixelDoF(x, y, _width, _height, &_rngBuffer[idx]);

		color = Scene::TraceRay(r, &_rngBuffer[idx], _recursions, _threshold, _ambientEmission, _sceneBuffer);

		if (_iteration == 1)
			_accumulationBuffer[idx] = color;
		else
			_accumulationBuffer[idx] = _accumulationBuffer[idx] + color;

		color = _accumulationBuffer[idx] / (float) _iteration;

		color = color.Pow(1.0f / 2.2f).Min(1.0f);

		color = color * 255.0f;

		_pixelBuffer.set(index<2>(_coordLookupBuffer[idx].Y, _coordLookupBuffer[idx].X), ConstructColor(
			(int) color.X,
			(int) color.Y,
			(int) color.Z,
			255));
	}
	);

	// We then restore these variables as they must be kept over iterations
	pixelBuffer = _pixelBuffer;
	accumulationBuffer = _accumulationBuffer;
	rngBuffer = _rngBuffer;
}

void PathTracer::GetPixelBuffer(int *buffer)
{
	copy(pixelBuffer, buffer);
}

int PathTracer::GetIteration()
{
	return iteration;
}

void PathTracer::Reset()
{
	iteration = 0;
}