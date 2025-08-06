#pragma once
#include "Game.h"
#include "PhaseParent.h"

class ActSelect : public PhaseParent
{
public:
	ActSelect();
	~ActSelect();

	void Initialize();
	void Update();
	void Draw();



private:
	Game::RenderData_Model center;

	Game::RenderData_Model ButtleIcon;
	Game::RenderData_Model GatyaIcon;
	Game::RenderData_Model UnitOverViewIcon;

	int iconSum_;

	int selectIcon = 0;

	Vector3 preCenterRotate;
	Vector3 targetCenterRotate;
	float easingFrame = 60;
	float easingFrameMAX = 60;
};

