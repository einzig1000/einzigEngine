#pragma once
#include "Game.h"
#include "MapCollider.h"

class Itemslot;

class Player : public MapCollider
{
public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Draw();

	void AddItemToItemslot(int itemID);

	// 視線レイ
	Line viewLine_;

	// 速度
	float speed_ = 0.15f;
	int wHeldFrames_ = 0;
	float normalSpeed_ = 0.15f;
	float dashSpeed_ = 0.3f;
	int preDash_ = 0;
	bool dash_ = false;

	// レティクル
	RenderData_Sprite reticle_;

	// プレイヤー
	RenderData_Model data_;

	// 足元４つのブロック上面座標
	Vector3 footBlockPositions_[4];

	// 所持アイテム
	Itemslot* Itemslot_;
};

