#pragma once
#include "Game.h"
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"

class PhaseParent
{
public:

	PHASE GetNextPhase();

protected:
	PHASE nextPhase;
};

