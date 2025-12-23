#pragma once
#include "Game.h"
#include "BaseCharactor.h"

class Itemslot;

class Player : public BaseCharactor
{
public:
	Player();
	~Player();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void UpdateViewLine();	// 視線レイ更新
	void UpdateMove();		// 移動更新
	void UpdateDush();		// ダッシュ更新
	void UpdateJump();		// ジャンプ更新

	// アイテムスロットにアイテムを追加
	void AddItemToItemslot(int itemID);

	// 視線レイ
	Line viewLine_;

	// 速度
	float speed_ = 0.15f;
	float normalSpeed_ = 0.13f;
	float dashSpeed_ = 0.20f;

	// ダッシュ関連
	int wHeldFrames_ = 0;
	int dashBufferTimer_ = 0;
	bool dash_ = false;

	// レティクル
	RenderData_Sprite reticle_;

	// 足元４つのブロック上面座標
	Vector3 footBlockPositions_[4];

	// 所持アイテム
	Itemslot* Itemslot_;
};

