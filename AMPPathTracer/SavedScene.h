#pragma once

#include "Types.h"
#include "Vector3.h"
#include "Camera.h"

#include <vector>
#include <string>

#include <json-cpp.hpp>

class SavedScene
{
public:
	SavedScene()
	{
		Recursions = 10;
		Threshold = 0.0f;
	}

	template<typename T>
	inline void serialize(jsoncpp::Stream<T>& stream)
	{
		fields(*this, stream,
			JSON_NVP(Recursions),
			JSON_NVP(Threshold),
			JSON_NVP(Cam),
			JSON_NVP(SceneEmission),
			JSON_NVP(Objects)
			);
	}

	int Recursions;
	float Threshold;

	Camera Cam;
	Vector3 SceneEmission;
	std::vector<SceneObject> Objects;

	static SavedScene LoadFromFile(std::string filename);
};