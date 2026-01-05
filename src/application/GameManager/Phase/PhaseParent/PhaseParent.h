#pragma once
#include "Game.h"
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"

class PhaseParent
{
public:
	virtual ~PhaseParent() = default;

	virtual void Initialize() = 0;
	virtual	void Update() = 0;
	virtual	void Draw() = 0;
	virtual void DrawImGui() = 0;

	virtual void ChangePhase(PHASE phase);
	virtual PHASE GetNextPhase();

protected:
	PHASE nextPhase_ = PHASE::Phase_None;
};

