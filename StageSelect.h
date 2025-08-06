#pragma once
#include "Game.h"
#include "PhaseParent.h"

class StageSelect : public PhaseParent
{
public:
	StageSelect();
	~StageSelect();

	void Initialize();
	void Update();
	void Draw();



private:

};

