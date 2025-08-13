#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "definition/definition.h"
#include <unordered_map>
#include <tuple>

enum class EaseType
{
	LINEAR,
	IN_SINE,
	OUT_SINE,
	IN_OUT_SINE,
	IN_QUAD,
	OUT_QUAD,
	IN_OUT_QUAD,
	IN_CUBIC,
	OUT_CUBIC,
	IN_OUT_CUBIC,
	IN_QUART,
	OUT_QUART,
	IN_OUT_QUART,
	IN_QUINT,
	OUT_QUINT,
	IN_OUT_QUINT,
	IN_EXPO,
	OUT_EXPO,
	IN_OUT_EXPO,
	IN_CIRC,
	OUT_CIRC,
	IN_OUT_CIRC,
	IN_BACK,
	OUT_BACK,
	IN_OUT_BACK,
	IN_ELASTIC,
	OUT_ELASTIC,
	IN_OUT_ELASTIC,
	IN_BOUNCE,
	OUT_BOUNCE,
};

struct EaseState
{
	float currentFrame = 0.0f;
	float SpendFrame = 1.0f;
};

// std::tupleのカスタムハッシュ関数をstd::namespace内に特化
namespace std
{
	// Vector3のテンプレート完全特殊化 (typename Tは不要です)
	template <>
	struct hash<Vector3>
	{
		size_t operator()(const Vector3& v) const
		{
			size_t h1 = hash<float>{}(v.x);
			size_t h2 = hash<float>{}(v.y);
			size_t h3 = hash<float>{}(v.z);
			// 3つのハッシュ値を組み合わせる
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	// std::tuple<float, float, EaseType>のテンプレート完全特殊化
	template <>
	struct hash<std::tuple<float, float, EaseType>>
	{
		size_t operator()(const std::tuple<float, float, EaseType>& t) const
		{
			size_t h1 = hash<float>{}(std::get<0>(t));
			size_t h2 = hash<float>{}(std::get<1>(t));
			size_t h3 = hash<EaseType>{}(std::get<2>(t));
			// 3つのハッシュ値を組み合わせる
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	// std::tuple<Vector3, Vector3, EaseType>のテンプレート完全特殊化
	template <>
	struct hash<std::tuple<Vector3, Vector3, EaseType>>
	{
		size_t operator()(const std::tuple<Vector3, Vector3, EaseType>& t) const
		{
			size_t h1 = hash<Vector3>{}(std::get<0>(t));
			size_t h2 = hash<Vector3>{}(std::get<1>(t));
			size_t h3 = hash<EaseType>{}(std::get<2>(t));
			// 3つのハッシュ値を組み合わせる
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

class Easings
{
public:
	static float EasingFloat(float start, float end, EaseType easeType, float SpendFrame);
	static Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float SpendFrame);
	static int COLOR(int startColor, int endColor, float t);
	
	static float F_LINEAR(float start, float end, float t);
	static float F_IN_SINE(float start, float end, float t);
	static float F_OUT_SINE(float start, float end, float t);
	static float F_IN_OUT_SINE(float start, float end, float t);
	static float F_IN_QUAD(float start, float end, float t);
	static float F_OUT_QUAD(float start, float end, float t);
	static float F_IN_OUT_QUAD(float start, float end, float t);
	static float F_IN_CUBIC(float start, float end, float t);
	static float F_OUT_CUBIC(float start, float end, float t);
	static float F_IN_OUT_CUBIC(float start, float end, float t);
	static float F_IN_QUART(float start, float end, float t);
	static float F_OUT_QUART(float start, float end, float t);
	static float F_IN_OUT_QUART(float start, float end, float t);
	static float F_IN_QUINT(float start, float end, float t);
	static float F_OUT_QUINT(float start, float end, float t);
	static float F_IN_OUT_QUINT(float start, float end, float t);
	static float F_IN_EXPO(float start, float end, float t);
	static float F_OUT_EXPO(float start, float end, float t);
	static float F_IN_OUT_EXPO(float start, float end, float t);
	static float F_IN_CIRC(float start, float end, float t);
	static float F_OUT_CIRC(float start, float end, float t);
	static float F_IN_OUT_CIRC(float start, float end, float t);
	static float F_IN_BACK(float start, float end, float t);
	static float F_OUT_BACK(float start, float end, float t);
	static float F_IN_OUT_BACK(float start, float end, float t);
	static float F_IN_ELASTIC(float start, float end, float t);
	static float F_OUT_ELASTIC(float start, float end, float t);
	static float F_IN_OUT_ELASTIC(float start, float end, float t);
	static float F_IN_BOUNCE(float start, float end, float t);
	static float F_OUT_BOUNCE(float start, float end, float t);

private:
	static std::unordered_map<std::tuple<float, float, EaseType>, EaseState> easeStates_Float;
	static std::unordered_map<std::tuple<Vector3, Vector3, EaseType>, EaseState> easeStates_Vector3;

	static float CalculateEasedValue(float start, float end, EaseType easeType, float t);

};