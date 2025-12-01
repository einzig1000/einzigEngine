#pragma once
#include <chrono>

class FixFPS
{
public:
	FixFPS();
	void UpdateFixFPS();
	float GetDeltaTime() const { return deltaTime_; }
	float GetRealFPS() const { return realTimeFPS_; }
	float GetAverageFPS() const { return averageFPS_; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	float targetFPS_;

	float deltaTime_;

	float averageFPS_;
	float realTimeFPS_;

};