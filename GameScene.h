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

	void Initialize(int stageNum);
	void Update();
	void Draw();


	enum class GameScenePhase
	{
		None,
		Setup,          // 駒の配置フェーズ
		PlayerTurn,     // プレイヤーの操作フェーズ
		EnemyTurn,      // 敵の操作フェーズ
		Result          // リザルト
	};

	void Initialize_Setup();
	void Initialize_PlayerTurn();
	void Initialize_EnemyTurn();
	void Initialize_Result();

	void Updata_Setup();
	void Updata_PlayerTurn();
	void Updata_EnemyTurn();
	void Updata_Result();



private:
	// フェーズ
	GameScenePhase phase_;
	GameScenePhase targetphase_;

	// キャラ一覧
	CharacterManager* characterManager_;

	// マップ
	Map* map_;

	// カメラ移動関連
	int frame_camera;
	int frameMAX_camera;
	Vector2int centerIndex_camera;

	// キー
	bool R;
	bool L;
	bool T;
	bool B;

	// 掴んでるキャラは何番目？
	int holdNumber;
	// キャラ配置済かどうか
	bool isSet[100];


};

