#pragma once
#include "Game.h"
#include <queue>	// std::priority_queue

enum class Turn
{
	None,
	Player,
	EnemyConsider,
	Enemy
};

enum class BlockType
{
	Empty = 0,		// 空（通行可能）
	Wall = 1,		// 壁（通行不可）
	Asid = 2,		// 毒（歩行コスト倍増）
	WarpIn = 3,		// ワープ入口
	WarpOut = 4,	// ワープ出口
};

enum class AstarBlockState
{
	None = 0,		// なんもしてない
	Explored = 1,	// クローズリスト（すでに最適な経路で処理済み）
	Path = 2,		// 最短経路
	Frontier = 3,	// オープンリスト（探索待ちのノード）
};

struct Block
{
	uint32_t model;
	uint32_t texture;
	Transforms transforms;
	Vector3 pivot = { 0,0,0 };
	uint32_t color;
	DrawOptions options;
	// AABB
	AABB AABB;
	// 歩行コスト
	int cost = 0;
	// 壁？
	BlockType type = BlockType::Empty;
	// 壁？
	AstarBlockState state = AstarBlockState::None;
	// タイプ変更後
	bool changeFlag = 0;
	// マウスのターゲットか否か
	bool mouseTarget = 0;

	void Draw()
	{
		Game::Drawobj(transforms, pivot, model, texture, color, options);
	}
};

// A*アルゴリズムのノード構造体
// 優先度キューの要素として使われるため、比較演算子のオーバーロードが必要
struct Node
{
	Vector2int index;
	int gCost; // スタート地点からの実際のコスト
	int hCost; // 目的地までの推定コスト（ヒューリスティックコスト）
	int fCost; // gCost + hCost (合計コスト)
	Vector2int parentIndex; // 経路再構築のための親ノードのインデックス
	Direction fromDir = Direction::None; // 親から見た方向

	// 優先度キューのための比較演算子（fCostが小さい方が優先されるようにする）
	bool operator>(const Node& other) const
	{
		if (fCost != other.fCost) return fCost > other.fCost;
		if (gCost != other.gCost) return gCost > other.gCost;
		// 方向優先度（例：Up > Right > Down > Left）
		return static_cast<int>(fromDir) > static_cast<int>(other.fromDir);
	}
};

struct ActPattern
{
	//敵１に向かって最短経路、
	//敵２に向かって最短経路、
	//・・・
	//有利ポジ１に向かって最短経路、
	//有利ポジ２に向かって最短経路、
	//・・・
	//より複数にあたる攻撃
	//・・・
	//単体への攻撃、
	//・・・
	//味方へのバフ

};

struct SkillInfo
{
	uint32_t cost; // 使用するために必要なコスト
	uint32_t delayCost; // 使用できない期間(必殺技のみ)

	uint32_t 〇〇;// 効果範囲

	uint32_t skillType;		// 攻撃、回復、バフ、etc...スキルのタイプ
	uint32_t skillOdds;		// スキルそのものの倍率
	uint32_t skillLevel;	// ゲーム外で上げれるパラメータ。上げる程効果アップ。レベル＊効果量
	// スキル効果量(回復量、ダメージ量等) = キャラ攻撃力 * skillOdds * skillLevel * キャラバフ + 固定値バフ
	std::string skillName;	// スキル識別タグ
};

// 名前
enum class Skill
{
	None,
};
enum class PassiveSkill
{
	None,
	攻撃範囲＋１,
	コスト20％削減,
	攻撃力等のステータスアップ系,
};
enum class AdvancedSkill
{
	None,
};
enum class UltimateSkill
{
	None,
};

struct Charactor
{
	uint32_t model;
	uint32_t texture;
	Transforms transforms;
	Vector3 pivot = { 0,0,0 };
	uint32_t color;
	DrawOptions options;

	uint32_t skill;
	uint32_t advancedSkill;
	uint32_t passiveSkill;
	uint32_t ultimateSkill;

	uint32_t moveRange; //移動可能範囲

	uint32_t actionDelay; //（技を使用する度に技コスト分加算される。場にいるキャラでこの数字が最も少ないキャラが最初に行動する）;


	uint32_t HP;
	uint32_t Attack;

	void Draw()
	{
		Game::Drawobj(transforms, pivot, model, texture, color, options);
	}
};

class TR2Class
{
public:
	TR2Class();
	void Initialize();

	void Update();
	void Draw();

	void Initialize_PlayerTurn();
	void Initialize_EnemyConsiderTurn();
	void Initialize_EnemyTurn();

	void Update_PlayerTurn();
	void Update_EnemyConsiderTurn();
	void Update_EnemyTurn();

	void Draw_PlayerTurn();
	void Draw_EnemyConsiderTurn();
	void Draw_EnemyTurn();

	bool translateBlock(float EasingMax, Transforms& transforms);

	// 座標からマップ上のインデックスを求める関数
	Vector2int PositionToIndex(Vector3 pos);
	// マップ上のインデックスから座標を求める関数
	Vector3 IndexToPosition(Vector2int index);

	// A*変数初期化
	void AstarSet(const Vector3& pos, const Vector3& target);

	// A*
	void Astar(const Vector3& pos, const Vector3& target);
	// 最短経路のマス数
	int GetShortestPathLength(const Vector3& pos, const Vector3& target);

private:
	// ターン
	Turn turn;
	Turn turnRepuest;

	// 移動経路
	std::vector<Vector2int> pathNodes;
	int pathStepIndex = 0;
	// 移動先選択中
	Vector3 moveTarget;
	Vector2int moveFrom;
	// 移動中
	bool movement;
	float movementT;
	float movementTmax;
	// 移動先
	Vector2int nextIndex;
	Vector3 nextBlockTranslate;
	// 移動元
	Vector2int fromIndex;
	Vector3 fromBlockTranslate;
	Vector3 fromBlockRotate;
	// 移動方向
	Direction direction;



	// プレイヤー
	Charactor player_;

	// エネミー
	Charactor enemy_;

	// ブロック
	Block block[MAP_HEIGHT][MAP_WIDTH];

	// 天球
	Transforms skyDomeTransforms;
	DrawOptions skyDomeOptions;


	//---------------------------model&texture------------------------
	int uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	int playerModel = Game::LoadOBJ("resources/player", "player.obj");
	int blockModel = Game::LoadOBJ("resources/block", "map.obj");
	int blockPng = Game::LoadTexture("resources/block/map.png");
	int skyDomeModel = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	int skyDomePng = Game::LoadTexture("resources/skyDome/skyDome.png");


	//------------------------------skill----------------------------
	std::vector<SkillInfo>skillList;
	std::vector<SkillInfo>advancedSkillList;
	std::vector<SkillInfo>passiveSkillList;
	std::vector<SkillInfo>ultimateSkillList;



	//-------------------------------A*-----------------------------


	// A*アルゴリズムに必要なデータ構造
	// 1. オープンリスト（これから評価する可能性のあるマスのリスト。fが最も小さいマスが優先）
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;

	// 2. 各ノードのgCostを保持（訪問済みかどうかの確認と、より良い経路が見つかったかの判断
	std::vector<std::vector<int>> gCostMap;

	// 3. 経路再構築のための親ノードのマップ
	std::map<Vector2int, Vector2int> parentMap;

	// A*スタートとゴール
	Vector2int startIndex;
	Vector2int targetIndex;
	bool aStarActive;



};

