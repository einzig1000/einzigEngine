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
	~GameScenePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:

	// プレイヤー
	std::unique_ptr<Player> player_;

	// マップ
	std::unique_ptr<MapManager> map_;

};