#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"
#include <memory>

class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }



private:
	std::unique_ptr<RenderObject> renderObject1_;
	Transforms transform1_;
	Vector4 color1_;

	uint32_t audio1;
	uint32_t audio2;
};