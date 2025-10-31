#pragma once
#include "Phase/PhaseParent/PhaseParent.h"

class Player;

class GameScenePhase :
	public PhaseParent
{
public:
	~GameScenePhase();
	void Initialize();
	void Update();
	void Draw();


private:
};

