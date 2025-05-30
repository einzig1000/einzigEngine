#pragma once
#include "definition.h"
#define MAP_WIDTH 10
#define MAP_HEIGHT 8

enum BlockType
{
	Empty = 0,        // 空
	Wall = 1,         // 壁
	Explored = 2,     // 探索済み
	Frontier = 3,     // 探索待ち（探索のフロンティア、次に探索する候補）
};

struct Block
{
	// SRV
	Transforms Transforms;
	// カラー
	int Color;
	// AABB
	AABB AABB;
	// 歩行コスト
	int cost;
	// 壁？
	BlockType Type;
	// タイプ変更後
	bool changeFlag;
	// マウスのターゲットか？
	bool mouseTraget;
};

class CG2_05
{
public:
	CG2_05();
	void Initialize();

	void Update();
	void Draw();

	void Astar(BlockType& map);

private:
	// テクスチャ
	int uvCheckerPng;
	int blockPng;
	// モデル
	int playerModel;
	int blockModel;


	// ブロック
	Block block[MAP_HEIGHT][MAP_WIDTH];
	bool editMode;

	// プレイヤー
	Transforms playerTransforms;
	int playerColor;
	AABB playerAABB;

	// 敵
	Transforms enemyTransforms;
	int enemyrColor;
	AABB enemyAABB;
	Vector2int targetBlosk;

	// カメラ操作
	Vector3 cameraRotate;
	Vector3 cameraCenter;
	float cameraDistance;

	// マウス
	Vector2 mousePos;
	bool PrePressMouse;
};

