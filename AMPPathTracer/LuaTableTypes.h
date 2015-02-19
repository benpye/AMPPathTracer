#pragma once

#include "luatables.h"

#include "Vector3.h"
#include "Camera.h"
#include "SavedScene.h"

#include <vector>

#define LUA_TABLE_GETTER(type) template<> \
	type LuaTableNode::getDefault<type>(const type &defaultValue) \
	{ \
		type result = defaultValue; \
		if(exists()) \
		{ \
			LuaTable customTable = stackQueryTable();

#define LUA_TABLE_GETTER_END } \
		stackRestore(); \
		return result; \
	}

#define LUA_TABLE_GETTER_MEMBER(x, y, z) result.y = customTable[#y].getDefault<x>(z);

#define LUA_TABLE_SETTER(type) template<> \
	void LuaTableNode::set<type>(const type &value) \
	{ \
		LuaTable customTable = stackCreateLuaTable();

#define LUA_TABLE_SETTER_END stackRestore(); \
	}

#define LUA_TABLE_SETTER_MEMBER(x) customTable[#x] = value.x;

#define LUA_TABLE_CLASS(x, y) \
	

template<>
Vector3 LuaTableNode::getDefault<Vector3>(const Vector3 &defaultValue)
{
	Vector3 result = defaultValue;

	if (exists())
	{
		LuaTable customTable = stackQueryTable();

		result.X = customTable[1].getDefault<float>(0.0f);
		result.Y = customTable[2].getDefault<float>(0.0f);
		result.Z = customTable[3].getDefault<float>(0.0f);
	}

	stackRestore();

	return result;
}

template<>
void LuaTableNode::set<Vector3>(const Vector3 &value)
{
	LuaTable customTable = stackCreateLuaTable();

	customTable[1] = value.X;
	customTable[2] = value.Y;
	customTable[3] = value.Z;

	stackRestore();
}

LUA_TABLE_GETTER(Camera)
LUA_TABLE_GETTER_MEMBER(Vector3, Origin, (Vector3{ 0.0f, 0.0f, 0.0f }))
LUA_TABLE_GETTER_MEMBER(float, Pitch, 0.0f)
LUA_TABLE_GETTER_MEMBER(float, Yaw, 0.0f)
LUA_TABLE_GETTER_MEMBER(float, Fov, 0.0f)
LUA_TABLE_GETTER_MEMBER(float, FocalPoint, 10.0f)
LUA_TABLE_GETTER_MEMBER(float, Aperture, 0.1f)
LUA_TABLE_GETTER_END

LUA_TABLE_SETTER(Camera)
LUA_TABLE_SETTER_MEMBER(Origin)
LUA_TABLE_SETTER_MEMBER(Pitch)
LUA_TABLE_SETTER_MEMBER(Yaw)
LUA_TABLE_SETTER_MEMBER(Fov)
LUA_TABLE_SETTER_MEMBER(FocalPoint)
LUA_TABLE_SETTER_MEMBER(Aperture)
LUA_TABLE_SETTER_END

LUA_TABLE_GETTER(Material)
LUA_TABLE_GETTER_MEMBER(Vector3, Emission, (Vector3{ 0.0f, 0.0f, 0.0f }))
LUA_TABLE_GETTER_MEMBER(Vector3, DiffuseColor, (Vector3{ 0.0f, 0.0f, 0.0f }))
LUA_TABLE_GETTER_MEMBER(Vector3, SpecularColor, (Vector3{ 1.0f, 1.0f, 1.0f }))
LUA_TABLE_GETTER_MEMBER(float, Reflectivity, 0.0f)
LUA_TABLE_GETTER_MEMBER(float, Transmittance, 0.0f)
LUA_TABLE_GETTER_MEMBER(float, RefractiveIndex, 1.0f)
LUA_TABLE_GETTER_MEMBER(float, Roughness, 0.0f)
LUA_TABLE_GETTER_END

LUA_TABLE_SETTER(Material)
LUA_TABLE_SETTER_MEMBER(Emission)
LUA_TABLE_SETTER_MEMBER(DiffuseColor)
LUA_TABLE_SETTER_MEMBER(SpecularColor)
LUA_TABLE_SETTER_MEMBER(Reflectivity)
LUA_TABLE_SETTER_MEMBER(Transmittance)
LUA_TABLE_SETTER_MEMBER(RefractiveIndex)
LUA_TABLE_SETTER_MEMBER(Roughness)
LUA_TABLE_SETTER_END

template<>
SceneObject LuaTableNode::getDefault<SceneObject>(const SceneObject &defaultValue)
{
	SceneObject result = defaultValue;

	if (exists())
	{
		LuaTable customTable = stackQueryTable();

		result.Type = customTable["Type"].getDefault<int>(OBJECT_INVALID);
		result.Properties = customTable["Properties"].getDefault<Material>(Material());

		switch (result.Type)
		{
		case OBJECT_SPHERE:
			result.Sphere.Origin = customTable["Origin"];
			result.Sphere.Radius = customTable["Radius"];
			break;
		case OBJECT_PLANE:
			result.Plane.Normal = customTable["Normal"];
			result.Plane.Point = customTable["Point"];
			break;
		}
	}

	stackRestore();

	return result;
}

template<>
void LuaTableNode::set<SceneObject>(const SceneObject &value)
{
	LuaTable customTable = stackCreateLuaTable();

	customTable["Type"] = value.Type;
	customTable["Properties"] = value.Properties;

	switch (value.Type)
	{
	case OBJECT_SPHERE:
		customTable["Origin"] = value.Sphere.Origin;
		customTable["Radius"] = value.Sphere.Radius;
		break;
	case OBJECT_PLANE:
		customTable["Normal"] = value.Plane.Normal;
		customTable["Point"] = value.Plane.Point;
		break;
	}

	stackRestore();
}

template<>
SavedScene LuaTableNode::getDefault<SavedScene>(const SavedScene &defaultValue)
{
	SavedScene result = defaultValue;

	if (exists())
	{
		LuaTable customTable = stackQueryTable();

		result.Cam = customTable["Camera"].getDefault<Camera>(Camera());
		result.Recursions = customTable["Recursions"].getDefault<int>(10);
		result.SceneEmission = customTable["SceneEmission"].getDefault<Vector3>({ 0.0f, 0.0f, 0.0f });
		result.Threshold = customTable["Threshold"].getDefault<float>(0.0f);

		auto keys = customTable["Objects"].keys();
		for (auto key : keys)
		{
			if (key.type == LuaKey::Type::Integer)
				result.Objects.push_back(customTable["Objects"][key.int_value]);
			else
				result.Objects.push_back(customTable["Objects"][key.string_value.c_str()]);
		}
	}

	stackRestore();

	return result;
}

template<>
void LuaTableNode::set<SavedScene>(const SavedScene &value)
{
	LuaTable customTable = stackCreateLuaTable();

	customTable["Camera"] = value.Cam;
	customTable["Recursions"] = value.Recursions;
	customTable["SceneEmission"] = value.SceneEmission;
	customTable["Threshold"] = value.Threshold;

	int i = 1;

	for (auto obj : value.Objects)
	{
		customTable["Objects"][i] = obj;
		i++;
	}

	stackRestore();
}