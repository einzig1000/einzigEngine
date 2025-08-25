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
		Check,			// 行動キャラがどちらの陣営かたしかめる
		PlayerTurn,     // プレイヤーの操作フェーズ
		EnemyTurn,      // 敵の操作フェーズ
		Result          // リザルト
	};



private:
	void Initialize_Setup();
	void Initialize_Check();
	void Initialize_PlayerTurn();
	void Initialize_EnemyTurn();
	void Initialize_Result();

	void Update_Setup();
	void Update_Check();
	void Update_PlayerTurn();
	void Update_EnemyTurn();
	void Update_Result();

	void Draw_Setup();
	void Draw_Check();
	void Draw_PlayerTurn();
	void Draw_EnemyTurn();
	void Draw_Result();

	void Update_FocusMode();

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
	// セットアップ完了してカメライージング中
	bool finishSetUp;

	// プレイヤーターンかエネミーターンの時に、盤面全体俯瞰モードか１キャラ注視モードか
	enum class CameraMode_Over_or_Focus
	{
		None,
		Over,
		Focus,
	};
	CameraMode_Over_or_Focus cameraMode = CameraMode_Over_or_Focus::None;
	void ChangeOver();
	void ChangeFocus();

	// プレイヤーターン時に選択している行動
	enum class CharactorSelectPattern
	{
		None,
		Attack,
		Move,
		ChangeCameraMode,
	};
	CharactorSelectPattern Act = CharactorSelectPattern::None;
	// 行動選択アイコン
	Game::RenderData_Model ActSelectIcon_[4];
	// 行動選択アイコンの目標座標
	Vector3 ActSelectIcon_targetIcon[4];
	// 移動先を示す半透明なやつの座標
	Vector3 moveTargetPosition;
	Vector2int moveTargetPositionIndex;
	// 移動先を示す半透明なやつの座標を連続で操作できないようにするクールタイム
	int moveTragetCoolTome;

};

