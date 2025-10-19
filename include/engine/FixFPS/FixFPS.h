#pragma once
#include <chrono>

class FixFPS
{
public:
	FixFPS();
	void UpdateFixFPS();
	float GetDeltaTime() const { return deltaTime_; }
	float GetFPS() const { return (deltaTime_ > 0.0f) ? (1.0f / deltaTime_) : 0.0f; }

private:
	std::chrono::steady_clock::time_point previousTime_;
	float deltaTime_ = 0.0f;
	const double targetFPS_ = 60;


};