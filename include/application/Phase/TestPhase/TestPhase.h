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
	Game::RenderData_Particle particle_;
	Game::RenderData_Model model_;

	int frame_;

};