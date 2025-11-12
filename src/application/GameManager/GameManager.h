#pragma once
#include <memory>
#include "Game.h"
#include "Phase/TestPhase/TestPhase.h"
#include "Phase/BattlePhase/BattlePhase.h"
#include "Phase/TitlePhase/TitlePhase.h"
#include "Phase/GameScenePhase/GameScenePhase.h"
#include "ResourceID.h"


class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();


private:
	// フェーズ
	PHASE phase_ = PHASE::Phase_None;
	PHASE requestPhase_ = PHASE::Phase_None;

	// フェーズクラス
	std::unique_ptr<TestPhase> testPhase_;
	std::unique_ptr<BattlePhase> battlePhase_;
	std::unique_ptr<TitlePhase> titlePhase_;
	std::unique_ptr<GameScenePhase> gameScenePhase_;

	// リソースID管理
	std::unique_ptr<ResourceID> resourceIDManager;
};

