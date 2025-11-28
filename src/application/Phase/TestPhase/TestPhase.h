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
	RenderData_Model wall1_;
	RenderData_Model wall2_;
	RenderData_Model wall3_;
	RenderData_Model wall4_;
	RenderData_Model player_;

	// 肩
	RenderData_Model shoulder_;
	// 肘
	RenderData_Model elbow_;
	// 手
	RenderData_Model hand_;

	RenderData_Rect rect_;

	RenderData_Sprite sprite1_;
	RenderData_Sprite sprite2_;

	RenderData_Particle3 particle1_;
	//RenderData_Particle3 particle2_;

	RenderData_Triangle triangle1_;
	RenderData_Triangle triangle2_;

	RenderData_Line line_;
	RenderData_Line line2_;
	RenderData_Line line3_;

	RenderData_MinecraftMap minecraftMap_;

	float roll_ = 0.0f;

	uint32_t audio1;
	uint32_t audio2;


};