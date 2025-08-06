#pragma once
#include "Game.h"
#include "PhaseParent.h"
#include "Map.h"

class CharacterManager;

class GameScene : public PhaseParent
{
public:
	GameScene(CharacterManager* characterManager);
	~GameScene();

	void Initialize();
	void Update();
	void Draw();



private:
	// キャラ一覧
	CharacterManager* characterManager_;

	// マップ
	Map* map_;
};

