#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "definition/definition.h"

class Easings
{
public:
	static float LINER(float start, float end, float t);
	static float IN_SINE(float start, float end, float t);
	static float OUT_SINE(float start, float end, float t);
	static float IN_OUT_SINE(float start, float end, float t);
	static float IN_QUAD(float start, float end, float t);
	static float OUT_QUAD(float start, float end, float t);
	static float IN_OUT_QUAD(float start, float end, float t);
	static float IN_CUBIC(float start, float end, float t);
	static float OUT_CUBIC(float start, float end, float t);
	static float IN_OUT_CUBIC(float start, float end, float t);
	static float IN_QUART(float start, float end, float t);
	static float OUT_QUART(float start, float end, float t);
	static float IN_OUT_QUART(float start, float end, float t);
	static float IN_QUINT(float start, float end, float t);
	static float OUT_QUINT(float start, float end, float t);
	static float IN_OUT_QUINT(float start, float end, float t);
	static float IN_EXPO(float start, float end, float t);
	static float OUT_EXPO(float start, float end, float t);
	static float IN_OUT_EXPO(float start, float end, float t);
	static float IN_CIRC(float start, float end, float t);
	static float OUT_CIRC(float start, float end, float t);
	static float IN_OUT_CIRC(float start, float end, float t);
	static float IN_BACK(float start, float end, float t);
	static float OUT_BACK(float start, float end, float t);
	static float IN_OUT_BACK(float start, float end, float t);
	static float IN_ELASTIC(float start, float end, float t);
	static float OUT_ELASTIC(float start, float end, float t);
	static float IN_OUT_ELASTIC(float start, float end, float t);
	static float IN_BOUNCE(float start, float end, float t);
	static float OUT_BOUNCE(float start, float end, float t);
	static int COLOR(int startColor, int endColor, float t);
};

