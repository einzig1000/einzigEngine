#pragma once
#include <externals/nlohmann/json.hpp>
#include "definition/definition.h"
using json = nlohmann::json;

class RenderData_Model;
class RenderData_Sprite;
class RenderData_Triangle;
class RenderData_Line;
class RenderData_Particle;

class JsonManager
{
public:
	static bool SaveToJson(RenderData_Particle& p, const std::string& path);
	static bool LoadFromJson(RenderData_Particle& p, const std::string& path);

private:
	static json Vec3ToJson(const Vector3& v);
	static Vector3 JsonToVec3(const json& j, const Vector3& def);
	static json Vec4ToJson(const Vector4& v);
	static Vector4 JsonToVec4(const json& j, const Vector4& def);
};

