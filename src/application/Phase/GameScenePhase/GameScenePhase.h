#pragma once
#include "Phase/PhaseParent/PhaseParent.h"
#include "Block/Block.h"

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
	std::unique_ptr<Player> player_;

	// マップ
	std::unique_ptr<MapManager> map_;
};