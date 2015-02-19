#include "SavedScene.h"

#include "luatables.h"

#include <fstream>

SavedScene SavedScene::LoadFromFile(std::string filename)
{
	SavedScene result;
	LuaTable ltable = LuaTable::fromFile(filename.c_str());

	result.Cam = ltable["Camera"];
	result.Recursions = ltable["Recursions"].getDefault<int>(10);
	result.SceneEmission = ltable["SceneEmission"].getDefault<Vector3>({ 0.0f, 0.0f, 0.0f });
	result.Threshold = ltable["Threshold"].getDefault<float>(0.0f);

	auto keys = ltable["Objects"].keys();
	for (auto key : keys)
	{
		if (key.type == LuaKey::Type::Integer)
			result.Objects.push_back(ltable["Objects"][key.int_value]);
		else
			result.Objects.push_back(ltable["Objects"][key.string_value.c_str()]);
	}

	ltable.destroyState();

	return result;
}