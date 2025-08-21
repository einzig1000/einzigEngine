#pragma once

#define WIDTH 1280
#define HEIGHT 720

#define STAGE_MAX 10

#define MAP_HEIGHT 10
#define MAP_WIDTH 20
#define BLOCK_HEIGHT 1.2f
#define BLOCK_WIDTH 1.2f

enum class ITEM
{
	Item_KingsSeed = 0,
	Item_QueensSeed = 1,
	Item_BishopsSeed = 2,
	Item_KnightsSeed = 3,
	Item_RooksSeed = 4,
	Item_PawnsSeed = 5,


};

enum class BLOCK_TYPE
{
	Empty = 0,		// 空（通行可能）
	Wall = 1,		// 壁（通行不可）
	Asid = 2,		// 毒（歩行コスト倍増）
	WarpIn = 3,		// ワープ入口
	WarpOut = 4,	// ワープ出口
};

enum class PHASE
{
	Phase_None,
	Phase_Title,
	Phase_ActSelect,//(ステージセレクト、キャラ一覧、ガチャ、から選択)
	// ステージセレクト選択以降
	Phase_StageSelect,
	Phase_GameScene,
	// キャラ一覧選択以降
	Phase_UnitOverview,
	// ガチャ以降
	Phase_Gatya,
	// shop以降
	Phase_Shop,
};

enum class TEXTURE
{
	// ブロック
	Map_Block = 0,

	// 駒
	Charactor_King = 1,
	Charactor_Queen = 2,
	Charactor_Bishop = 3,
	Charactor_Knight = 4,
	Charactor_Rook = 5,
	Charactor_Pawn = 6,

	// フェーズ選択用のアイコン
	Phase_Buttle = 7,
	Phase_Gatya = 8,
	Phase_UnitOverview = 9,

	// ステージセレクトのモノリス
	StageSelect_icon = 10,
};