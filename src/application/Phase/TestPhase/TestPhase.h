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
	RenderData_Model ground_;

	RenderData_Model player_;

	RenderData_Sprite sprite1_;
	RenderData_Sprite sprite2_;

	RenderData_Particle particle1_;
	RenderData_Particle particle2_;

	RenderData_Triangle triangle1_;
	RenderData_Triangle triangle2_;

	RenderData_Line line_;
	RenderData_Line line2_;
	RenderData_Line line3_;


	float roll_ = 0.0f;

	uint32_t audio1;
	uint32_t audio2;


};