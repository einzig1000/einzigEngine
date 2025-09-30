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
	Charactor* Clone() const
	{
		return new Charactor(*this);
	}
	~Charactor();

	void Update();

	// 3Dモデル
	Game::RenderData_Model data;
	// 移動先を表す半透明3Dモデル
	Game::RenderData_Model targetdata;
	
	// 細長いステータスシート
	Game::RenderData_Sprite dataSeat;
	// でかいステータスシート
	Game::RenderData_Sprite dataSeat_type;

	States states_default_;
	States states_buttle_;

	// 行動遅延
	// これが少ないキャラから行動する。行動する度に加算される。強い技ほど加算される
	float actionDelay = 0;

	static int getCharactorSum_;
	int serialNumber_;

	// true == player
	bool EnemyOrPlayer_ = true;

	Skill skill;
	Skill advancedSkill;
	Skill passiveSkill;
	Skill ultimateSkill;

	// 向き
	Direction  direction = Direction::Right;

	Vector2int pos;

private:

};

