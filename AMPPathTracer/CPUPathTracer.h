#pragma once

#include "Vector3.h"
#include "LCGRandom.h"
#include "Scene.h"
#include "Camera.h"

#include <d3d11.h>

class CPUPathTracer
{
public:
    CPUPathTracer(int _width, int _height);

    void InitBuffers();
    void LoadScene(std::vector<SceneObject> objects);
    void RenderIteration();
    int GetIteration();
    void Reset();

    ~CPUPathTracer();

    Camera Cam;
    int Recursions;
    ID3D11Texture2D *texture;

private:
    int width;
    int height;
    int iteration;

    V3 *accumulationBuffer;
    LCGRandom *rngBuffer;
    Coord *coordLookupBuffer;
    uint8_t *pixelBuffer;

    std::vector<SceneObject> sceneBuffer;
};

