#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "definition/definition.h"


class Easings
{
public:
	static float EasingFloat(float start, float end, EaseType easeType, float t);
	static Vector3 EasingVector3(Vector3 start, Vector3 end, EaseType easeType, float t);
	static int COLOR(int startColor, int endColor, float t);
	

private:
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

	static float CalculateEasedValue(float start, float end, EaseType easeType, float t);



};