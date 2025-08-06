#pragma once
#include "Game.h"
#include "PhaseParent.h"

class CharacterManager;
class Map;

class UnitOverview : public PhaseParent
{
public:
	UnitOverview(CharacterManager* characterManager);
	~UnitOverview();

	void Initialize();
	void Update();
	void Draw();



private:
	// キャラ一覧
	CharacterManager* characterManager_;

	// マップ
	Map* map_;

	// 注目してる列
	int targetY = 0;

	// フレーム毎ホイール量
	int preWheel = 0;
	int nowWheel = 0;
};

