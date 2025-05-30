#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "definition.h"

class Easings
{
public:
	static float LINER(int count, int countMax, float start, float end);
	static float IN_SINE(int count, int countMax, float start, float end);
	static float OUT_SINE(int count, int countMax, float start, float end);
	static float IN_OUT_SINE(int count, int countMax, float start, float end);
	static float IN_QUAD(int count, int countMax, float start, float end);
	static float OUT_QUAD(int count, int countMax, float start, float end);
	static float IN_OUT_QUAD(int count, int countMax, float start, float end);
	static float IN_CUBIC(int count, int countMax, float start, float end);
	static float OUT_CUBIC(int count, int countMax, float start, float end);
	static float IN_OUT_CUBIC(int count, int countMax, float start, float end);
	static float IN_QUART(int count, int countMax, float start, float end);
	static float OUT_QUART(int count, int countMax, float start, float end);
	static float IN_OUT_QUART(int count, int countMax, float start, float end);
	static float IN_QUINT(int count, int countMax, float start, float end);
	static float OUT_QUINT(int count, int countMax, float start, float end);
	static float IN_OUT_QUINT(int count, int countMax, float start, float end);
	static float IN_EXPO(int count, int countMax, float start, float end);
	static float OUT_EXPO(int count, int countMax, float start, float end);
	static float IN_OUT_EXPO(int count, int countMax, float start, float end);
	static float IN_CIRC(int count, int countMax, float start, float end);
	static float OUT_CIRC(int count, int countMax, float start, float end);
	static float IN_OUT_CIRC(int count, int countMax, float start, float end);
	static float IN_BACK(int count, int countMax, float start, float end);
	static float OUT_BACK(int count, int countMax, float start, float end);
	static float IN_OUT_BACK(int count, int countMax, float start, float end);
	static float IN_ELASTIC(int count, int countMax, float start, float end);
	static float OUT_ELASTIC(int count, int countMax, float start, float end);
	static float IN_OUT_ELASTIC(int count, int countMax, float start, float end);
	static float IN_BOUNCE(int count, int countMax, float start, float end);
	static float OUT_BOUNCE(int count, int countMax, float start, float end);
	static int COLOR(int count, int countMax, int startX, int endX);
};

