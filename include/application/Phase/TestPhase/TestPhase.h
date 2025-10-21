#pragma once
#include "Phase/PhaseParent/PhaseParent.h"

class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase();

	void Initialize();
	void Update();
	void Draw();

private:
	RenderData_Model model_;
	Game::RenderData_Particle particle_;
	RenderData_Sprite sprite_;
	RenderData_Triangle triangle_;
	RenderData_Line line_;
	RenderData_Line line2_;
	RenderData_Line line3_;

	int frame_;

	float roll_ = 0.0f;

};