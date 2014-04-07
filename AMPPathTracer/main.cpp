#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>

#include "Vector3.h"
#include "LCGRandom.h"
#include "Scene.h"
#include "Camera.h"
#include "Controller.h"

#include <amp.h>
#include <amp_math.h>
#include <amp_graphics.h>
#include <iostream>
#include <sstream>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

using namespace concurrency;
using namespace fast_math;
using namespace graphics;

int ConstructColor(int r, int g, int b, int a) restrict(amp, cpu)
{
	return (r & 0xFF) + ((g & 0xFF) << 8) + ((b & 0xFF) << 16) + ((a & 0xFF) << 24);
}

int main(int argc, char **argv)
{
	int width = 1024;
	int height = 768;
	int bounces = 10;

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-w"))
			width = atoi(argv[++i]);

		if (!strcmp(argv[i], "-h"))
			height = atoi(argv[++i]);
	}

	sf::RenderWindow window(sf::VideoMode(width, height), "Path tracer");

	sf::Texture pixelTexture;
	pixelTexture.create(width, height);

	sf::Sprite pixelSprite(pixelTexture);

	// Storage buffers
	int *cpuBuffer = new int[width * height];
	LCGRandom *random = new LCGRandom[width * height];
	Coord *screenCoords = new Coord[width * height];

	srand((unsigned int)time(NULL));

	for (int i = 0; i < width * height; i++)
	{
		random[i] = LCGRandom(rand());
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			screenCoords[x + y * width].X = x;
			screenCoords[x + y * width].Y = y;
		}
	}

	//array_view<int, 1> pixelBuffer(width * height, cpuBuffer);
    texture<int, 2> pixelBuffer(height, width, 32U);
	array<Vector3, 1> accumulation(width * height);
	array<LCGRandom, 1> randomNumbers(width * height, random);
	array<Coord, 1> screenPos(width * height, screenCoords);

	Scene s;
	s.Sky.Emission = Vector3(0.0f);

	s.Objects[0].Type = OBJECT_PLANE;
	s.Objects[0].Origin = Vector3(0.0f, 0.0f, -25.0f);
	s.Objects[0].Normal = Vector3(0.0f, 0.0f, 1.0f);
	s.Objects[0].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);

	s.Objects[1].Type = OBJECT_PLANE;
	s.Objects[1].Origin = Vector3(0.0f, 0.0f, 25.0f);
	s.Objects[1].Normal = Vector3(0.0f, 0.0f, -1.0f);
	s.Objects[1].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);

	s.Objects[2].Type = OBJECT_PLANE;
	s.Objects[2].Origin = Vector3(0.0f, 100.0f, 0.0f);
	s.Objects[2].Normal = Vector3(0.0f, -1.0f, 0.0f);
	s.Objects[2].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);

	s.Objects[3].Type = OBJECT_PLANE;
	s.Objects[3].Origin = Vector3(0.0f, -5.0f, 0.0f);
	s.Objects[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	s.Objects[3].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);

	s.Objects[4].Type = OBJECT_PLANE;
	s.Objects[4].Origin = Vector3(25.0f, 0.0f, 0.0f);
	s.Objects[4].Normal = Vector3(-1.0f, 0.0f, 0.0f);
	s.Objects[4].Properties.DiffuseColor = Vector3(0.25f, 0.75f, 0.25f);

	s.Objects[5].Type = OBJECT_PLANE;
	s.Objects[5].Origin = Vector3(-25.0f, 0.0f, 0.0f);
	s.Objects[5].Normal = Vector3(1.0f, 0.0f, 0.0f);
	s.Objects[5].Properties.DiffuseColor = Vector3(0.75f, 0.25f, 0.25f);

	s.Objects[6].Type = OBJECT_SPHERE;
	s.Objects[6].Origin = Vector3(0.0f, 60.0f, 524.9f);
	s.Objects[6].Radius = 500.0f;
	s.Objects[6].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);
	s.Objects[6].Properties.Emission = Vector3(4.0f, 4.0f, 4.0f);

	s.Objects[7].Type = OBJECT_SPHERE;
	s.Objects[7].Origin = Vector3(12.0f, 65.0f, -15.0f);
	s.Objects[7].Radius = 10.0f;
	s.Objects[7].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);
	s.Objects[7].Properties.Transmittance = 1.0f;
	s.Objects[7].Properties.Reflectivity = 1.0f;
	s.Objects[7].Properties.RefractiveIndex = 1.4f;

	s.Objects[8].Type = OBJECT_SPHERE;
	s.Objects[8].Origin = Vector3(-12.0f, 80.0f, -15.0f);
	s.Objects[8].Radius = 10.0f;
	s.Objects[8].Properties.DiffuseColor = Vector3(0.75f, 0.75f, 0.75f);
	s.Objects[8].Properties.Reflectivity = 1.0f;
	s.Objects[8].Properties.RefractiveIndex = 50.0f;

	// Performance monitor
	uint64_t totaltime = 0;
	int iteration = 0;

	sf::Font debugFont;
	if (!debugFont.loadFromFile("SourceCodePro-Regular.otf"))
	{
		std::cout << "Failed to load debug font" << std::endl;
		return 0;
	}

	sf::Text debugText = sf::Text("", debugFont, 12);

	sf::Clock frameClock = sf::Clock();

	bool showDebug = true;

	Camera cam;
	cam.CalculateDirection();
	cam.Fov = (80.0f / 180.0f) * M_PI;
	cam.CalculateScreenDistance(width);
	cam.FocalPoint = 10.0f;
	cam.Aperture = 0.01f;

    float contributionThreshold = 0.0f;

	auto resetRender = [&](){ totaltime = 0; iteration = 0; };

	Controller controls;
	controls.AddBind(sf::Keyboard::W,     sf::Keyboard::S,    0.000025f,   [&](float mult){ cam.Move(cam.Forward * mult); resetRender(); });
	controls.AddBind(sf::Keyboard::D,     sf::Keyboard::A,    0.000025f,   [&](float mult){ cam.Move(cam.Right * mult); resetRender(); });
	controls.AddBind(sf::Keyboard::Up,    sf::Keyboard::Down, 0.0000015f,  [&](float mult){ cam.ChangeDirection(mult, 0.0f); resetRender(); });
	controls.AddBind(sf::Keyboard::Right, sf::Keyboard::Left, 0.0000015f,  [&](float mult){ cam.ChangeDirection(0.0f, mult); resetRender(); });
	controls.AddBind(sf::Keyboard::Y,     sf::Keyboard::T,    0.00000001f, [&](float mult){ cam.Aperture = fmaxf(0.0f, cam.Aperture + mult); resetRender(); });
	controls.AddBind(sf::Keyboard::H,     sf::Keyboard::G,    0.00001f,    [&](float mult){ cam.FocalPoint = fmaxf(0.0f, cam.FocalPoint + mult); resetRender(); });
	controls.AddBind(sf::Keyboard::N,     sf::Keyboard::B,    0.000001f,   [&](float mult){ cam.Fov = fmaxf(0.0f, cam.Fov + mult); cam.CalculateScreenDistance(width); resetRender(); });
	controls.AddBind(sf::Keyboard::I,     sf::Keyboard::U,    0.0000001f,  [&](float mult){ contributionThreshold = fmaxf(0.0f, contributionThreshold + mult); resetRender(); });

	uint64_t time;

	bool focus = true;

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::O))
				showDebug = !showDebug;

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::C))
			{
				bounces = std::max(0, --bounces);
				resetRender();
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::V))
			{
				bounces = ++bounces;
				resetRender();
			}

			if (event.type == sf::Event::GainedFocus)
				focus = true;

			if (event.type == sf::Event::LostFocus)
				focus = false;
		}

		iteration++;

		parallel_for_each(
            randomNumbers.extent,
			[=, &randomNumbers, &screenPos, &accumulation, &pixelBuffer](index<1> idx) restrict(amp)
		{
			Vector3 color;

			// Offset x/y by up to half a pixel in each direction to give free AA
			float x = screenPos[idx].X + randomNumbers[idx].NextFloat() - 0.5f;
			float y = screenPos[idx].Y + randomNumbers[idx].NextFloat() - 0.5f;

			Ray r = cam.CalculateRayForPixelDoF(x, y, width, height, &randomNumbers[idx]);

			color = s.TraceRay(r, &randomNumbers[idx], bounces, contributionThreshold);

			if (iteration == 1)
				accumulation[idx] = color;
			else
				accumulation[idx] = accumulation[idx] + color;

			color = accumulation[idx] / (float) iteration;

			color = color.Pow(1.0f / 2.2f).Min(1.0f);

			color = color * 255.0f;

            pixelBuffer.set(index<2>(screenPos[idx].Y, screenPos[idx].X), ConstructColor(
				(int) color.X,
				(int) color.Y,
				(int) color.Z,
				255));
		}
		);

        copy(pixelBuffer, cpuBuffer);

		pixelTexture.update((sf::Uint8 *)cpuBuffer);

		window.draw(pixelSprite);

		time = frameClock.getElapsedTime().asMicroseconds();

		if (focus)
			controls.Update((float) time);

		frameClock.restart();
		totaltime += time;

		if (showDebug)
		{
			std::stringstream debugString;
			debugString << "Frametime: " << time / 1000.0f << "ms\n"
				<< "Avg: " << (totaltime / 1000.0f) / iteration << "ms\n"
				<< "Iterations: " << iteration << "\n"
				<< "Runtime: " << totaltime / 1000000.0f << "s\n"
				<< "Toggle Debug Overlay (O)\n"
				<< "Recursions (CV): " << bounces << "\n"
                << "Contribution Threshold (UI): " << contributionThreshold << "\n"
				<< "Camera Origin (WASD): " << cam.Origin.ToString() << "\n"
				<< "Camera Direction (Arrow Keys): " << cam.Forward.ToString() << "\n"
				<< "Camera Aperture (TY): " << cam.Aperture << "\n"
				<< "Camera Focal Point (GH): " << cam.FocalPoint << "\n"
				<< "Camera Fov (BN): " << (cam.Fov / M_PI) * 180.0f << "\n";

			debugText.setString(debugString.str());

			window.draw(debugText);
		}

		window.display();

	}

	return 0;
}