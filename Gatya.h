#pragma once
#include "Game.h"
#include "PhaseParent.h"

class CharacterManager;
class Charactor;

class Gatya : public PhaseParent
{
public:
	Gatya(CharacterManager* characterManager);
	~Gatya();

	void Initialize();
	void Update();
	//void 
	void Draw();

	void AddCharactor(Charactor* newChar);

private:
	CharacterManager* characterManager_;

};
