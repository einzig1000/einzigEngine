#pragma once
#include "Game.h"

enum class states
{

};

enum class CharactorName
{
	King,
	Queen,
	Bishop,
	Knight,
	Rook,
	Pawn,
};

class Charactor
{
public:
	Charactor(CharactorName name);
	~Charactor();

	void Update();
	void Draw();

private:

	Game::RenderData_Model data;


};