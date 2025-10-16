#pragma once
#include "Phase/PhaseParent/PhaseParent.h"


class BattlePhase
	: public PhaseParent
{
public:
	BattlePhase();
	~BattlePhase();

	void Initialize();
	void Update();
	void Draw();

private:

};

