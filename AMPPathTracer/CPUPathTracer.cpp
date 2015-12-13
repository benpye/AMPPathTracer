#include "Common.h"

#include "CPUPathTracer.h"

#include <d3d11.h>
#include <vector>
#include <omp.h>

extern concurrency::accelerator_view *g_av;
extern ID3D11Device*                  g_pd3dDevice;

int ConstructColor(int r, int g, int b, int a)
{
    return (r & 0xFF) + ((g & 0xFF) << 8) + ((b & 0xFF) << 16) + ((a & 0xFF) << 24);
}

CPUPathTracer::CPUPathTracer(int _width, int _height)
{
    width = _width;
    height = _height;

    iteration = 0;
}

CPUPathTracer::~CPUPathTracer()
{
    delete[] pixelBuffer;
    delete[] accumulationBuffer;
    delete[] rngBuffer;
    delete[] coordLookupBuffer;
}

void CPUPathTracer::InitBuffers()
{
    srand((int)time(NULL));

    rngBuffer = new LCGRandom[width * height];
    accumulationBuffer = new V3[width * height];
    pixelBuffer = new uint8_t[4 * width * height];
    coordLookupBuffer = new Coord[width * height];

    std::vector<LCGRandom> randomVector(width * height);
    std::generate(randomVector.begin(), randomVector.end(), []() { return LCGRandom(rand());});
    copy(randomVector.begin(), randomVector.end(), rngBuffer);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            coordLookupBuffer[y * width + x] = { x, y };
        }
    }
}

void CPUPathTracer::LoadScene(std::vector<SceneObject> objects)
{
    sceneBuffer = objects;
}

void CPUPathTracer::RenderIteration()
{
    iteration++;

    #pragma omp parallel for
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            V3 color;

            int idx = y * width + x;

            // Offset x/y by up to half a pixel in each direction to give free AA
            float rx = coordLookupBuffer[idx].X + rngBuffer[idx].NextFloat() - 0.5f;
            float ry = coordLookupBuffer[idx].Y + rngBuffer[idx].NextFloat() - 0.5f;

            Ray r = { Cam.Origin, Cam.CalculateDirectionForPixel(rx, ry, width, height) };

            color = Scene::TraceRayCPU(r, &rngBuffer[idx], Recursions, sceneBuffer);

            if (iteration == 1)
                accumulationBuffer[idx] = color;
            else
                accumulationBuffer[idx] = accumulationBuffer[idx] + color;

            color = accumulationBuffer[idx] / (float)iteration;

            color = color.Pow(1.0f / 2.2f).Min(1.0f);

            color = color * 255.0f;

            pixelBuffer[4 * (y * width + x)]     = (uint8_t)color.X;
            pixelBuffer[4 * (y * width + x) + 1] = (uint8_t)color.Y;
            pixelBuffer[4 * (y * width + x) + 2] = (uint8_t)color.Z;
            pixelBuffer[4 * (y * width + x) + 3] = 255;
        }
    }

    D3D11_TEXTURE2D_DESC td;
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Usage = D3D11_USAGE_DYNAMIC;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Height = height;
    td.Width = width;
    td.MipLevels = 1;
    td.MiscFlags = 0;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;

    D3D11_SUBRESOURCE_DATA sr;
    sr.pSysMem = pixelBuffer;
    sr.SysMemPitch = width * 4;
    sr.SysMemSlicePitch = width * height * 4;

    g_pd3dDevice->CreateTexture2D(&td, &sr, &texture);
}

int CPUPathTracer::GetIteration()
{
    return iteration;
}

void CPUPathTracer::Reset()
{
    iteration = 0;
}