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
	void Initialize();
	void Update();
	void Draw();


private:

	RenderData_Sprite worldSelect;
	RenderData_Sprite options;

	uint32_t frameCount = 0;

	TitlePhaseState currentState = TitlePhaseState::Title;
	TitlePhaseState nextState = TitlePhaseState::Title;
};
