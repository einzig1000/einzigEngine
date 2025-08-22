#pragma once
#include "Game.h"

enum class states
{

};

enum class CharactorName
{
	King,		// 場に一体しかだせない最強キャラ。なんらかのチートスキル持ち
	Queen,		// ヒーラー
	Bishop,		// バッファー
	Knight,		// アタッカー
	Rook,		// タンク
	Pawn,		// 
	CharactorMAX
};

class Charactor
{
public:
	Charactor(TEXTURE name);
	~Charactor();

	void Update();


	Game::RenderData_Model data;
	Game::RenderData_Sprite dataSeat;
	Game::RenderData_Sprite dataSeat_type;


	int power_;
	int hp_;
	int actPoint_;
	static int getCharactorSum_;
	int serialNumber_;

private:
};

