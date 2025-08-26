#pragma once
#include "Game.h"
#include <sstream>



class Map
{
public:
	Map();
	void Initialize();
	void Update();
	void Draw();

	// stageNum番号のステージを読み込む何回よみこんだっていい
	void LoadMap(int stageNum);
	// ブロックタイプに合った形状に変化させる
	void ShapeChangeByType(Vector2int index);


	// startからendまで移動した場合の最短歩数
	int shotestCost(Vector2int start, Vector2int end);
	// startからendまで移動した場合の最短経路
	std::vector<Vector2int> FindPath(Vector2int start, Vector2int end);


	// indexのブロックタイプを返す
	BLOCK_TYPE BlockTypeByIndex(Vector2int index);
	// indexのエフェクト状況を返す
	BLOCK_EFFECT_TYPE BlockEffectByIndex(Vector2int index);

	// 移動可能範囲を調べてEffectType[y][x] == BLOCK_EFFECT_TYPE::AbleCharactorSetにする
	void CheckAblemovement(Vector2int index, int idouhanni);

	// インデックスAからインデックスBはマップ的に移動できるか
	bool A_to_B(Vector2int start, Vector2int target);


	// マウスレイとの衝突判定
	bool IsCollisionMouseRay_[MAP_HEIGHT][MAP_WIDTH];


	// 描画用データ
	Game::RenderData_Model data[MAP_HEIGHT][MAP_WIDTH];
	// マップの種類情報
	BLOCK_TYPE blockType[MAP_HEIGHT][MAP_WIDTH];
	// マップのエフェクト情報
	BLOCK_EFFECT_TYPE EffectType[MAP_HEIGHT][MAP_WIDTH];
	// マップマス上のキャラの所属
	BLOCK_CHAR CharactorType[MAP_HEIGHT][MAP_WIDTH];

private:
	// ここに全部あるから変えてはいけない
	std::string mapCSV[STAGE_MAX];
	std::string mapEffectCSV[STAGE_MAX];

};