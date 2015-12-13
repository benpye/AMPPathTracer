//--------------------------------------------------------------------------------------
// File: SimpleSample.cpp
//
// This is a simple Win32 desktop application showing use of DirectXTK
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <d3d11.h>

#include <directxmath.h>

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

#include "BasicTimer.h"

#include "PathTracer.h"
#include "CPUPathTracer.h"

#include <algorithm>

#define RUN_GPU

using namespace DirectX;
using namespace DirectX::SimpleMath;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11DeviceContext*                g_pImmediateContext = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;
ID3D11RenderTargetView*             g_pRenderTargetView = nullptr;
ID3D11Texture2D*                    g_pDepthStencil = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;

std::unique_ptr<PrimitiveBatch<VertexPositionColor>>    g_Batch;
std::unique_ptr<SpriteBatch>                            g_Sprites;
std::unique_ptr<SpriteFont>                             g_Font;

#ifdef RUN_GPU
PathTracer                                             *g_PathTracer;
#else
CPUPathTracer                                          *g_PathTracer;
#endif
accelerator_view                                       *g_av;

BasicTimer                                              g_timer;


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
        return 0;

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDC_ICON);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"SampleWindowClass";
    wcex.hIconSm = LoadIcon(hInstance, IDC_ICON);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"SampleWindowClass", L"Simple Pathtracer", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}


void InitPathTracer()
{
#ifdef RUN_GPU
    g_PathTracer = new PathTracer(WINDOW_WIDTH, WINDOW_HEIGHT);
#else
    g_PathTracer = new CPUPathTracer(WINDOW_WIDTH, WINDOW_HEIGHT);
#endif

    std::vector<SceneObject> objects;

    SceneObject tmpObj;
    tmpObj.Type = 1;
    tmpObj.Plane.Point = { 0, 0, -25 };
    tmpObj.Plane.Normal = { 0, 0, 1 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 1;
    tmpObj.Plane.Point = { 0, 0, 25 };
    tmpObj.Plane.Normal = { 0, 0, -1 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 1;
    tmpObj.Plane.Point = { 0, 100, 0 };
    tmpObj.Plane.Normal = { 0, -1, 0 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 1;
    tmpObj.Plane.Point = { 0, -5, 0 };
    tmpObj.Plane.Normal = { 0, 1, 0 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 1;
    tmpObj.Plane.Point = { 25, 0, 0 };
    tmpObj.Plane.Normal = { -1, 0, 0 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .25, .75, .25 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 1;
    tmpObj.Plane.Point = { -25, 0, 0 };
    tmpObj.Plane.Normal = { 1, 0, 0 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .25, .25 };
    tmpObj.Properties.Emission = { 0, 0,0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 0;
    tmpObj.Sphere.Radius = 500;
    tmpObj.Sphere.Origin = { 0, 60, 524.9f };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 4, 4, 4 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    tmpObj.Type = 0;
    tmpObj.Sphere.Radius = 10;
    tmpObj.Sphere.Origin = { 12, 65, -15 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0, 0 };
    tmpObj.Properties.Reflectivity = 1;
    objects.push_back(tmpObj);

    tmpObj.Type = 0;
    tmpObj.Sphere.Radius = 10;
    tmpObj.Sphere.Origin = { -12, 65, -15 };
    tmpObj.Properties.SpecularColor = { 1, 1, 1 };
    tmpObj.Properties.DiffuseColor = { .75, .75, .75 };
    tmpObj.Properties.Emission = { 0, 0, 0 };
    tmpObj.Properties.Reflectivity = 0;
    objects.push_back(tmpObj);

    g_PathTracer->Recursions = 10;

    g_PathTracer->InitBuffers();

    g_PathTracer->LoadScene(objects);
    g_PathTracer->Cam.Fov = 1.3962634016f;
    g_PathTracer->Cam.Yaw = 0;
    g_PathTracer->Cam.Pitch = 0;
    g_PathTracer->Cam.Origin = { 0, 0, 0 };
    g_PathTracer->Cam.CalculateDirection();
    g_PathTracer->Cam.CalculateScreenDistance(WINDOW_WIDTH);
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_av = new accelerator_view(Concurrency::direct3d::create_accelerator_view(g_pd3dDevice));
    bool ret = accelerator::set_default(g_av->accelerator.device_path);

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
    if (FAILED(hr))
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Create DirectXTK objects
    g_Sprites.reset(new SpriteBatch(g_pImmediateContext));

    g_Font.reset(new SpriteFont(g_pd3dDevice, L"italic.spritefont"));

    InitPathTracer();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pDepthStencil) g_pDepthStencil->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    g_timer.Update();

    //
    // Clear the back buffer
    //
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    g_Sprites->Begin(SpriteSortMode_Deferred);

    g_PathTracer->RenderIteration();
#ifdef RUN_GPU
    IUnknown *unk = concurrency::graphics::direct3d::get_texture(g_PathTracer->pixelBuffer);
#endif
    ID3D11ShaderResourceView *texSRV = nullptr;
#ifdef RUN_GPU
    ID3D11Texture2D *tex = reinterpret_cast<ID3D11Texture2D *>(unk);
#else
    ID3D11Texture2D *tex = g_PathTracer->texture;
#endif
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_view_desc;

    shader_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    shader_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_view_desc.Texture2D.MipLevels = -1;
    shader_view_desc.Texture2D.MostDetailedMip = 0;

    HRESULT hr = g_pd3dDevice->CreateShaderResourceView(tex, &shader_view_desc, &texSRV);
    g_Sprites->Draw(texSRV, XMFLOAT2(0, 0), nullptr, Colors::White);
    tex->Release();
    wchar_t str[64];
    swprintf(str, L"%s\nIteration time (ms): %f\nSamples: %i\n",
#ifdef RUN_GPU
        L"GPU MODE",
#else
        L"CPU MODE",
#endif
        (g_timer.GetTotal() / g_PathTracer->GetIteration()) * 1000.0f, g_PathTracer->GetIteration());
    g_Font->DrawString(g_Sprites.get(), str, XMFLOAT2(10, 10), Colors::Yellow);
    g_Sprites->End();

    g_pSwapChain->Present(0, 0);
}