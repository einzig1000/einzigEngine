#include "Easings.h"

/// <summary>
/// イージング関数
/// </summary>
/// <param name="count">現在のカウント</param>
/// <param name="countMax">カウントの最大値</param>
/// <param name="start">イージングの始点</param>
/// <param name="end">イージングの終点</param>
/// <returns>start ～ end の間でイージングした値</returns>
/// 
float Easings::LINER(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	return (1.0f - t) * start + t * end;
}
float Easings::IN_SINE(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - cosf((t * float(M_PI)) / 2.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_SINE(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = sinf((t * float(M_PI)) / 2.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_SINE(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = -(cosf(float(M_PI) * t) - 1.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_QUAD(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t * t;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_QUAD(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - (1.0f - t) * (1.0f - t);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_QUAD(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f ? 2.0f * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_CUBIC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t * t * t;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_CUBIC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - powf(1.0f - t, 3.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_CUBIC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f ? 4.0f * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_QUART(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t * t * t * t;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_QUART(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - powf(1.0f - t, 4.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_QUART(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f ? 8.0f * t * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_QUINT(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t * t * t * t * t;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_QUINT(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - powf(1.0f - t, 5.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_QUINT(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f ? 16.0f * t * t * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_EXPO(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 0.0f ? 0.0f
		: powf(2.0f, 10.0f * t - 10.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_EXPO(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 1.0f ? 1.0f
		: 1.0f - powf(2.0f, -10.0f * t);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_EXPO(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_CIRC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_CIRC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_CIRC(int count, int countMax, float start, float end)
{
	float t = 0;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f
		? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f
		: (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_BACK(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c1 = 1.70158f;
	static const float c3 = c1 + 1.0f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = c3 * t * t * t - c1 * t * t;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_BACK(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c1 = 1.70158f;
	static const float c3 = c1 + 1.0f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_BACK(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c1 = 1.70158f;
	static const float c2 = c1 * 1.525f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t < 0.5f
		? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
		: (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_ELASTIC(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c4 = (2.0f * float(M_PI)) / 3.0f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::OUT_ELASTIC(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c4 = (2.0f * float(M_PI)) / 3.0f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_OUT_ELASTIC(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float c5 = (2.0f * float(M_PI)) / 4.5f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f
		? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
		: (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
	return (1.0f - easedT) * start + easedT * end;
}
float Easings::IN_BOUNCE(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float n1 = 7.5625f;
	static const float d1 = 2.75f;
	if (countMax == 0)return 1.0f;
	else t = 1.0f - (float(count) / float(countMax));

	float easedT = 0.0f;
	if (t < 1.0f / d1)easedT = n1 * t * t;
	else if (t < 2 / d1)easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;
	else if (t < 2.5 / d1)easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
	else easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;

	return (1.0f - easedT) * end + easedT * start;
}
float Easings::OUT_BOUNCE(int count, int countMax, float start, float end)
{
	float t = 0;
	static const float n1 = 7.5625f;
	static const float d1 = 2.75f;
	if (countMax == 0)return 1.0f;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	float easedT = 0.0f;
	if (t < 1.0f / d1)easedT = n1 * t * t;
	else if (t < 2 / d1)easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;
	else if (t < 2.5 / d1)easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
	else easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;
	return (1.0f - easedT) * start + easedT * end;
}
int Easings::COLOR(int count, int countMax, int startX, int endX)
{
	float t = 0;
	float x = 0;
	if (countMax == 0)return 1;
	else
	{
		t = float(count) / float(countMax);
		if (t > 1.0f)t = 1.0f;
		else if (t < 0.0f)t = 0.0f;
	}
	x = (1.0f - t) * startX + t * endX;


	return int(x);
}