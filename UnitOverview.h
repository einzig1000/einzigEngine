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

	enum class ViewPhase
	{
		None,
		ALL,		// 全キャラ表示中
		ALL_UNIT,	// 全キャラ表示->単体表示の演出中
		UNIT,		// 単体表示中
		UNIT_ALL,	// 単体表示->全キャラ表示の演出中
	};

	void Initialize_ALL();
	void Initialize_ALL_UNIT();
	void Initialize_UNIT();
	void Initialize_UNIT_ALL();

	void Update_ALL();
	void Update_ALL_UNIT();
	void Update_UNIT();
	void Update_UNIT_ALL();


private:
	// フェーズ
	ViewPhase phase_;
	ViewPhase targetphase_;

	// キャラ一覧
	CharacterManager* characterManager_;

	// マップ
	Map* map_;
	// マップのイージング前と後
	Transforms mapPreTransforms[MAP_HEIGHT][MAP_WIDTH];
	Transforms mapTargetTransforms[MAP_HEIGHT][MAP_WIDTH];
	int t;
	float tMAX = 5;

	// 注目してる列
	int targetY = 0;

	// キャラ詳細画面に移行する時フォーカスしているキャラナンバー
	int targetChar = 0;

	// 詳細画面の回転中心
	Game::RenderData_Model center;
	// 回転量
	float deltaRotate = 0;
	// 回転中心のイージング前と後
	Transforms centerPreTransforms;
	Transforms centerTargetTransforms;


};

