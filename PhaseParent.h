#pragma once
#include "Game.h"
#include "enum.h"

class PhaseParent
{
public:

	PHASE ChangePhase();

protected:
	PHASE nextPhase;
};

