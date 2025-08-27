#pragma once

#define WIDTH 1280
#define HEIGHT 720

#define STAGE_MAX 10

#define MAP_HEIGHT 10
#define MAP_WIDTH 20
#define BLOCK_HEIGHT 1.2f
#define BLOCK_WIDTH 1.2f

#define MOVE_COST 20.0f

enum class ITEM
{
	Item_KingsSeed = 0,
	Item_QueensSeed = 1,
	Item_BishopsSeed = 2,
	Item_KnightsSeed = 3,
	Item_RooksSeed = 4,
	Item_PawnsSeed = 5,


};

struct States
{
	// 諸々のパワー
	// アタッカーなら与えるダメージが変わるしヒーラーなら回復量が変わる
	float power = 0;
	// HP
	float hp = 0;
	float hp_max = 10;
	// 素早さ
	// 速いと技をactionDelay少なく使える
	float speed = 0;
	// 移動可能距離
	int movePoint = 5;
};

enum class BLOCK_TYPE
{
	Empty = 0,		// 空
	Wall = 1,		// 壁
	stairs = 2,		// 階段
};

enum class BLOCK_CHAR
{
	Empty,		// 空
	OnEnemy,	// 敵がいる
	OnPlayer,	// 味方がいる
};

enum class BLOCK_EFFECT_TYPE
{
	Empty = 0,		// 空
	移動可能,		// キャラ配置可能を示すエフェクト
	攻撃範囲,		// 攻撃予定地
};

enum class SkillAreaShape
{
	円,
	直線,
	十字,
	正方形,
	前方正方形,//(正面)
	例外,
};
enum class SkillType
{
	攻撃,
	回復,
	バフ,
};
enum class SkillTarget
{
	単体,
	全体,
	自身,
};
enum class AllStates
{
	power,
	hp_max,
	speed,
	moveRenge,
	例外,
};

struct Skill
{
	// スキルのタイプ
	SkillType type;

	// スキル効果がある範囲
	int range;
	// スキル対象
	SkillTarget target;
	// スキル範囲の形
	SkillAreaShape areaShape;
	// スキル倍率
	float multiplier;

	// 使用コスト
	float cost;

	// 壁越しでも効果があるか
	bool passWall;
	// 高低差があっても効果があるか
	bool passHeight;

	// バフがかかるステータス(スキルのタイプがバフ系だった場合のみ)
	AllStates buffTargetStates;
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
	Charactor_King,
	Charactor_Queen,
	Charactor_Bishop,
	Charactor_Knight,
	Charactor_Rook,
	Charactor_Pawn,

	// フェーズ選択用のアイコン
	Phase_Buttle,
	Phase_Gatya,
	Phase_UnitOverview,

	// ステージセレクトのモノリス
	StageSelect_icon,

	// バトル中の行動選択アイコン
	ActSelect_Icon_None,
	ActSelect_Icon_Skill,
	ActSelect_Icon_Move,
	ActSelect_Icon_ChangeCameraMode,

	// 攻撃エフェクト
	Attack_Effect,

	// 攻撃対象
	Attack_Marker,

	// 以下スプライト

	// キャラクターシート
	Charactor_Seat,
	Charactor_SeatMini,
	// 2D駒
	CharactorSprite_King,
	CharactorSprite_Queen,
	CharactorSprite_Bishop,
	CharactorSprite_Knight,
	CharactorSprite_Rook,
	CharactorSprite_Pawn,
};


//enum class Skill_
//{
//	None,
//	正面に攻撃,
//	４方向１マス攻撃,
//
//	自身のＨＰ５％回復,
//};
//enum class PassiveSkill
//{
//	None,
//	攻撃範囲＋１,
//	移動範囲＋１,
//
//	行動コスト10％減少,
//	攻撃力＋10,
//	素早さ＋10,
//	最大HP＋10,
//
//	毎ターンHP10回復,
//};
//enum class AdvancedSkill_
//{
//	None,
//
//	// 攻撃系
//	範囲攻撃の術_1マス_単体_平面,//1マス以内の敵から１人選び攻撃
//	範囲攻撃の術_2マス_単体_平面,//2マス以内の敵から１人選び攻撃
//	範囲攻撃の術_3マス_単体_平面,//3マス以内の敵から１人選び攻撃
//	範囲攻撃の術_1マス_全体_平面,//1マス以内の敵を全員攻撃
//	範囲攻撃の術_2マス_全体_平面,//2マス以内の敵を全員攻撃
//	範囲攻撃の術_3マス_全体_平面,//3マス以内の敵を全員攻撃
//	直線攻撃の術_1マス_単体_1方向_非貫通,//
//	壁または敵にあたるまで直線攻撃,
//	正面に攻撃,
//	４方向１マス攻撃,
//	円状１マス以内の攻撃,
//	円状２マス以内の攻撃,
//	円状３マス以内の攻撃,
//	盤面全体攻撃,
//
//	// 回復系
//	範囲回復の術_1マス_単体,//1マス以内の味方から１人選び回復
//	範囲回復の術_2マス_単体,//2マス以内の味方から１人選び回復
//	範囲回復の術_3マス_単体,//3マス以内の味方から１人選び回復
//	範囲回復の術_1マス_全体,//1マス以内の味方を全員回復
//	範囲回復の術_2マス_全体,//2マス以内の味方を全員回復
//	範囲回復の術_3マス_全体,//3マス以内の味方を全員回復
//	回復の術_単体,			//位置に関わらず１人選び回復
//	回復の術_自身,			//自身のＨＰ回復
//
//
//	// バフ系
//	自身の攻撃力＋10,
//	自身の素早さ＋10,
//	自身の最大HP＋10,
//	自身の移動可能範囲＋1,
//
//};
//enum class UltimateSkill
//{
//	None,
//
//	全味方全回復,
//
//
//
//	盤面全体攻撃,
//};