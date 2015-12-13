#include "Common.h"

#include "PathTracer.h"

#include <vector>

extern concurrency::accelerator_view *g_av;

PathTracer::PathTracer(int _width, int _height)
    : pixelBuffer(_height, _width, 8U, *g_av),
    accumulationBuffer(_width * _height, *g_av),
    rngBuffer(_width * _height, *g_av),
    coordLookupBuffer(_width * _height, *g_av),
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
    srand((int)time(NULL));

    std::vector<LCGRandom> randomVector(width * height);
    std::generate(randomVector.begin(), randomVector.end(), []() { return LCGRandom(rand());});
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
    sceneBuffer = new array<SceneObject, 1>(objects.size(), objects.begin(), objects.end(), *g_av);
}

void PathTracer::RenderIteration()
{
    iteration++;

    // We have to make local copies because C++AMP disallows access to class members (grr)
    //auto _pixelBuffer        = pixelBuffer;
    auto _accumulationBuffer = accumulationBuffer;
    auto _rngBuffer          = rngBuffer;
    auto _coordLookupBuffer  = coordLookupBuffer;
    auto _sceneBuffer        = *sceneBuffer;

    Camera _cam = Cam;

    V3 _origin = _cam.Origin;

    int _width = width;
    int _height = height;
    int _iteration = iteration;
    int _recursions = Recursions;

    texture_view<unorm_4, 2> wo_pixelBuffer(pixelBuffer);

    parallel_for_each(
        *g_av,
        bufferSize,
        [=, &_accumulationBuffer, &_rngBuffer, &_coordLookupBuffer, &_sceneBuffer](index<1> idx) restrict(amp)
    {
        V3 color;

        // Offset x/y by up to half a pixel in each direction to give free AA
        float x = _coordLookupBuffer[idx].X + _rngBuffer[idx].NextFloat() - 0.5f;
        float y = _coordLookupBuffer[idx].Y + _rngBuffer[idx].NextFloat() - 0.5f;

        Ray r = { _origin, _cam.CalculateDirectionForPixel(x, y, _width, _height) };

        color = Scene::TraceRay(r, &_rngBuffer[idx], _recursions, _sceneBuffer);

        if (_iteration == 1)
            _accumulationBuffer[idx] = color;
        else
            _accumulationBuffer[idx] = _accumulationBuffer[idx] + color;

        color = _accumulationBuffer[idx] / (float)_iteration;

        color = color.Pow(1.0f / 2.2f).Min(1.0f);

        //pixelBuffer.set(idx, { (int)color.X, (int)color.Y, (int)color.Z });
        wo_pixelBuffer.set(index<2>(_coordLookupBuffer[idx].Y, _coordLookupBuffer[idx].X), unorm_4{ color.X, color.Y, color.Z, 1.0f });
    }
    );

    // We then restore these variables as they must be kept over iterations
    accumulationBuffer = _accumulationBuffer;
    rngBuffer = _rngBuffer;
}

int PathTracer::GetIteration()
{
    return iteration;
}

void PathTracer::Reset()
{
    iteration = 0;
}