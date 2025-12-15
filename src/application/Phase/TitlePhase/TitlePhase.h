#pragma once
#include "Phase/PhaseParent/PhaseParent.h"

enum class TitlePhaseState
{
	None,
	Title,
	WorldSelect,
	Options,
};

class TitlePhase :
	public PhaseParent
{
public:
	TitlePhase();
	~TitlePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:

	std::unique_ptr<RenderData_Sprite> worldSelect;
	std::unique_ptr<RenderData_Sprite> options;

	uint32_t frameCount = 0;

	TitlePhaseState currentState = TitlePhaseState::Title;
	TitlePhaseState nextState = TitlePhaseState::Title;
};
