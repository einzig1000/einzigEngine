#pragma once
#include "Phase/PhaseParent/PhaseParent.h"
#include "Block.h"

class Player;
class MapManager;

class GameScenePhase :
	public PhaseParent
{
public:
	GameScenePhase();
	~GameScenePhase();
	void Initialize();
	void Update();
	void Draw();


private:

	// プレイヤー
	Player* player_;

	// マップ
	MapManager* map_;


};