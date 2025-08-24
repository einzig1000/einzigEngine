#pragma once
#include "Game.h"

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

	States states_default_;
	States states_buttle_;

	// 行動遅延
	// これが少ないキャラから行動する。行動する度に加算される。強い技ほど加算される
	int actionDelay = 0;

	static int getCharactorSum_;
	int serialNumber_;

private:
};

