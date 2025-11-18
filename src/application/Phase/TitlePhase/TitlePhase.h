#pragma once
#include "Phase/PhaseParent/PhaseParent.h"

class TitlePhase :
	public PhaseParent
{
public:
	void Initialize();
	void Update();
	void Draw();


private:

	RenderData_Sprite warldSelect;
	RenderData_Sprite options;

};
