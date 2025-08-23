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

	// ブロックタイプに合った形状に変化させる
	void ShapeChangeByType(Vector2int index);

	// stageNum番号のステージを読み込む何回よみこんだっていい
	void LoadMap(int stageNum);

	// 座標変更したっていい
	void SetTransforms(Vector2int index, Transforms transforms) { data[index.x][index.y].transforms = transforms; }

	BLOCK_TYPE BlockTypeByIndex(Vector2int index);

	BLOCK_EFFECT_TYPE BlockEffectByIndex(Vector2int index);


	Game::RenderData_Model data[MAP_HEIGHT][MAP_WIDTH];

	// インデックスAからインデックスBはマップ的に移動できるか
	bool A_to_B(Vector2int start, Vector2int target);

	// マップのエフェクト情報
	BLOCK_EFFECT_TYPE EffectType[MAP_HEIGHT][MAP_WIDTH];
private:
	// マップの種類情報
	BLOCK_TYPE blockType[MAP_HEIGHT][MAP_WIDTH];

	std::string mapCSV[STAGE_MAX];
	std::string mapEffectCSV[STAGE_MAX];
};