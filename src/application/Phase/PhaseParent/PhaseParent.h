#pragma once
#include "Game.h"
#include "definition/definition.h"

class PhaseParent
{
public:

	PHASE GetNextPhase();

protected:
	PHASE nextPhase;
};

