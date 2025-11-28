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


	std::unique_ptr<RenderData_Model> ground_;
	std::unique_ptr<RenderData_Model> wall1_;
	std::unique_ptr<RenderData_Model> wall2_;
	std::unique_ptr<RenderData_Model> wall3_;
	std::unique_ptr<RenderData_Model> wall4_;
	std::unique_ptr<RenderData_Model> player_;

	std::unique_ptr<RenderData_Model> shoulder_;
	std::unique_ptr<RenderData_Model> elbow_;
	std::unique_ptr<RenderData_Model> hand_;

	std::unique_ptr<RenderData_Rect> rect_;

	std::unique_ptr<RenderData_Sprite> sprite1_;
	std::unique_ptr<RenderData_Sprite> sprite2_;

	std::unique_ptr<RenderData_Particle> particle1_;

	std::unique_ptr<RenderData_Triangle> triangle1_;
	std::unique_ptr<RenderData_Triangle> triangle2_;

	std::unique_ptr<RenderData_Line> line_;
	std::unique_ptr<RenderData_Line> line2_;
	std::unique_ptr<RenderData_Line> line3_;

	float roll_ = 0.0f;

	uint32_t audio1;
	uint32_t audio2;


};