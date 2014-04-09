#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>

#include "Vector3.h"
#include "Scene.h"
#include "Camera.h"
#include "Controller.h"
#include "PathTracer.h"
#include "SavedScene.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <json-cpp.hpp>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

using namespace concurrency;
using namespace fast_math;
using namespace graphics;

void printUsage()
{
	std::cout << "AMPPathTracer.exe <input> [options]" << std::endl;
	std::cout << "Options: " << std::endl;
	std::cout << "   -w <width>  - Sets the width of the rendered image" << std::endl;
	std::cout << "   -h <height> - Sets the height of the rendered image" << std::endl;
}

bool fileExists(std::string filename)
{
	std::ifstream infile(filename);
	return infile.good();
}

int main(int argc, char **argv)
{
	int width = 1024;
	int height = 768;

	if (argc < 2)
	{
		std::cout << "Must specify scene input file" << std::endl;
		printUsage();
		return 0;
	}

	std::string scenefile(argv[1]);

	if (!fileExists(scenefile))
	{
		std::cout << "Scene input file not found" << std::endl;
		return 0;
	}

	SavedScene scene;

	try
	{
		scene = SavedScene::LoadFromFile(scenefile);
	}
	catch (...)
	{
		std::cout << "Error parsing json file" << std::endl;
		return 0;
	}

	for (int i = 2; i < argc; i++)
	{
		if (!strcmp(argv[i], "-w"))
			width = atoi(argv[++i]);

		else if (!strcmp(argv[i], "-h"))
			height = atoi(argv[++i]);

		else
		{
			std::cout << "Invalid argument: " << argv[i] << std::endl;
			printUsage();
			return 0;
		}
	}

	sf::RenderWindow window(sf::VideoMode(width, height), "Path tracer");

	sf::Texture pixelTexture;
	pixelTexture.create(width, height);

	sf::Sprite pixelSprite(pixelTexture);

	// Performance monitor
	uint64_t totaltime = 0;

	sf::Font debugFont;
	if (!debugFont.loadFromFile("SourceCodePro-Regular.otf"))
	{
		std::cout << "Failed to load debug font" << std::endl;
		return 0;
	}

	bool showDebug = true;

	int *cpuBuffer = new int[width * height];

	PathTracer p(width, height);

	p.Recursions = scene.Recursions;
	p.Threshold = scene.Threshold;

	p.InitBuffers();
	p.LoadScene(scene.Objects);

	p.Cam = scene.Cam;
	p.Cam.CalculateDirection();
	p.Cam.CalculateScreenDistance(width);

	auto resetRender = [&](){ totaltime = 0; p.Reset(); };

	Controller controls;
	controls.AddBind(sf::Keyboard::W,     sf::Keyboard::S,    0.000025f,   [&](float mult){ p.Cam.Move(p.Cam.Forward * mult); resetRender(); });
	controls.AddBind(sf::Keyboard::D,     sf::Keyboard::A,    0.000025f,   [&](float mult){ p.Cam.Move(p.Cam.Right * mult); resetRender(); });
	controls.AddBind(sf::Keyboard::Up,    sf::Keyboard::Down, 0.0000015f,  [&](float mult){ p.Cam.ChangeDirection(mult, 0.0f); resetRender(); });
	controls.AddBind(sf::Keyboard::Right, sf::Keyboard::Left, 0.0000015f,  [&](float mult){ p.Cam.ChangeDirection(0.0f, mult); resetRender(); });
	controls.AddBind(sf::Keyboard::Y,     sf::Keyboard::T,    0.00000001f, [&](float mult){ p.Cam.Aperture = fmaxf(0.0f, p.Cam.Aperture + mult); resetRender(); });
	controls.AddBind(sf::Keyboard::H,     sf::Keyboard::G,    0.00001f,    [&](float mult){ p.Cam.FocalPoint = fmaxf(0.0f, p.Cam.FocalPoint + mult); resetRender(); });
	controls.AddBind(sf::Keyboard::N,     sf::Keyboard::B,    0.000001f,   [&](float mult){ p.Cam.Fov = fmaxf(0.0f, p.Cam.Fov + mult); p.Cam.CalculateScreenDistance(width); resetRender(); });
	controls.AddBind(sf::Keyboard::I,     sf::Keyboard::U,    0.0000001f,  [&](float mult){ p.Threshold = fmaxf(0.0f, p.Threshold + mult); resetRender(); });

	bool focus = true;

	sf::Text debugText = sf::Text("", debugFont, 12);

	sf::Clock frameClock = sf::Clock();
	uint64_t time;

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
				p.Recursions = std::max(0, --p.Recursions);
				resetRender();
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::V))
			{
				p.Recursions = ++p.Recursions;
				resetRender();
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::P))
				pixelTexture.copyToImage().saveToFile("screenshot.png");

			if (event.type == sf::Event::GainedFocus)
				focus = true;

			if (event.type == sf::Event::LostFocus)
				focus = false;
		}

		p.RenderIteration();

		p.GetPixelBuffer(cpuBuffer);

		pixelTexture.update((sf::Uint8 *)cpuBuffer);

		window.draw(pixelSprite);

		if (focus)
			controls.Update((float) time);

		time = frameClock.getElapsedTime().asMicroseconds();

		frameClock.restart();
		totaltime += time;

		int iteration = p.GetIteration();

		if (showDebug)
		{
			std::stringstream debugString;
			debugString << "Frametime: " << time / 1000.0f << "ms\n"
				<< "Avg: " << (totaltime / 1000.0f) / iteration << "ms\n"
				<< "Iterations: " << iteration << "\n"
				<< "Runtime: " << totaltime / 1000000.0f << "s\n"
				<< "Toggle Debug Overlay (O)\n"
				<< "Save Screenshot (P)\n"
				<< "Recursions (CV): " << p.Recursions << "\n"
				<< "Contribution Threshold (UI): " << p.Threshold << "\n"
				<< "Camera Origin (WASD): " << p.Cam.Origin.ToString() << "\n"
				<< "Camera Direction (Arrow Keys): " << p.Cam.Forward.ToString() << "\n"
				<< "    Pitch: " << (p.Cam.Pitch / M_PI) * 180.0f << "\n"
				<< "    Yaw: " << (p.Cam.Yaw / M_PI) * 180.0f << "\n"
				<< "Camera Aperture (TY): " << p.Cam.Aperture << "\n"
				<< "Camera Focal Point (GH): " << p.Cam.FocalPoint << "\n"
				<< "Camera Fov (BN): " << (p.Cam.Fov / M_PI) * 180.0f << "\n";

			debugText.setString(debugString.str());

			window.draw(debugText);
		}

		window.display();

	}

	return 0;
}