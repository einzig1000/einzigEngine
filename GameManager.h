#pragma once
#include <memory>

#include "Title.h"
#include "ActSelect.h"
#include "StageSelect.h"
#include "GameScene.h"
#include "UnitOverview.h"
#include "Gatya.h"

#include "CharacterManager.h"

#include "enum.h"


class GameManager
{
public:
	GameManager();
	~GameManager();

	void Update();
	void Draw();


private:
	// 所持キャラ
	CharacterManager* characterManager_;

	// 所持アイテム

	// 天球


	// フェーズ
	PHASE phase_ = PHASE::Phase_None;
	PHASE requestPhase_ = PHASE::Phase_None;

	Title* title_;
	ActSelect* actSelect_;
	StageSelect* stageSelect_;
	GameScene* gameScene_;
	UnitOverview* unitOverview_;
	Gatya* gatya_;


};

