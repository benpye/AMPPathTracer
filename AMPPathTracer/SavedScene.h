#pragma once

#include "Types.h"
#include "Vector3.h"
#include "Camera.h"

#include <vector>
#include <string>

class SavedScene
{
public:
	SavedScene()
	{
		Recursions = 10;
		Threshold = 0.0f;
	}

	int Recursions;
	float Threshold;

	Camera Cam;
	Vector3 SceneEmission;
	std::vector<SceneObject> Objects;

	static SavedScene LoadFromFile(std::string filename);
};