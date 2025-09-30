#pragma once
#include "Game.h"
#include "PhaseParent.h"

class Title: public PhaseParent
{
public:
	Title();
	~Title();

	void Initialize();
	void Update();
	void Draw();

private:


};

