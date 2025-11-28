#pragma once
#include <externals/nlohmann/json.hpp>
#include "definition/definition.h"
#include <vector>
#include <string>

using json = nlohmann::json;

class RenderData_Model;
class RenderData_Sprite;
class RenderData_Triangle;
class RenderData_Line;
class RenderData_Particle;
class RenderData_Particle3;

class JsonManager
{
public:
	static bool SaveToJson(RenderData_Particle& data, const std::string& path);
	static bool LoadFromJson(RenderData_Particle& data, const std::string& path);

	static bool SaveToJson(RenderData_Particle3& data, const std::string& path);
	static bool LoadFromJson(RenderData_Particle3& data, const std::string& path);

	static bool SaveToJson(RenderData_Model& data, const std::string& path);
	static bool LoadFromJson(RenderData_Model& data, const std::string& path);

	static bool SaveToJson(RenderData_Line& data, const std::string& path);
	static bool LoadFromJson(RenderData_Line& data, const std::string& path);

	static bool SaveToJson(RenderData_Triangle& data, const std::string& path);
	static bool LoadFromJson(RenderData_Triangle& data, const std::string& path);

	static bool SaveToJson(RenderData_Sprite& data, const std::string& path);
	static bool LoadFromJson(RenderData_Sprite& data, const std::string& path);



	static bool SaveToJson(const std::string& path, const std::string key, const int& data);
	static bool SaveToJson(const std::string& path, const std::string key, const float& data);
	static bool SaveToJson(const std::string& path, const std::string key, const std::string& data);
	static bool SaveToJson(const std::string& path, const std::string key, const Vector2int& data);
	static bool SaveToJson(const std::string& path, const std::string key, const Vector2& data);
	static bool SaveToJson(const std::string& path, const std::string key, const Vector3& data);
	static bool SaveToJson(const std::string& path, const std::string key, const Vector4& data);
	static bool SaveToJson(const std::string& path, const std::string key, const AABB& data);

	static int LoadFromJson(const std::string& path, const std::string key);

private:

	std::vector<json> jsons;

	static json ToJson(const int& data);
	static json ToJson(const float& data);
	static json ToJson(const std::string& data);
	static json ToJson(const Vector2int& data);
	static json ToJson(const Vector2& data);
	static json ToJson(const Vector3& data);
	static json ToJson(const Vector4& data);
	static json ToJson(const AABB& data);
	static json ToJson(const Matrix3x3& data);
	static json ToJson(const Matrix4x4& data);

	static int ToInt(const json& j);
	static float ToFloat(const json& j);
	static std::string ToString(const json& j);
	static Vector2int ToVector2int(const json& j);
	static Vector2 ToVector2(const json& j);
	static Vector3 ToVector3(const json& j);
	static Vector4 ToVector4(const json& j);
	static AABB ToAABB(const json& j);
	static Matrix3x3 ToMatrix3x3(const json& j);
	static Matrix4x4 ToMatrix4x4(const json& j);


	static json SetJsonValue(const std::string& key, const float& data);
	// str文字列をdelimiterで分割する
	static std::vector<std::string> SplitString(const std::string& str, char delimiter);
};
