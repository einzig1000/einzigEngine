#pragma once
#include "Map.h"
#include <memory>

enum class Phase
{
	None,
	Title,
	ActSelect,//(ステージセレクト、キャラ一覧、から選択)
	// ステージセレクト選択以降
	StageSelect,
	GameScene,
	// キャラ一覧選択以降
	UnitOverview,
	// 
};

class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();


private:

	Map* map_;

	Phase phase_ = Phase::None;
	Phase requestPhase_ = Phase::None;


};

