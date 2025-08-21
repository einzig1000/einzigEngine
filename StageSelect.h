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

	int GetSelectStage() const { return selectIcon + 1; };

private:
	Game::RenderData_Model center;

	Game::RenderData_Model Icon_[STAGE_MAX];

	int iconSum_;

	int selectIcon = 0;

	Vector3 preCenterRotate;
	Vector3 targetCenterRotate;
	float easingFrame = 60;
	float easingFrameMAX = 60;

};

