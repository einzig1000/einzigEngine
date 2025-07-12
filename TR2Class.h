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
	Slope = 2,		// 坂（Wallへの階段）
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
	// A*判定
	AstarBlockState state = AstarBlockState::None;
	// タイプ変更後
	bool changeFlag = 0;
	// マウスのターゲットか否か
	bool mouseTarget = 0;

	bool isAttackTarget = false;// 攻撃予定地;
	bool isHealTarget = false;	// 回復予定地;
	bool isBuffTarget = false;	// バフ予定地;

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
	bool fromSlope = false; // 直前がAsidかどうか

	// 優先度キューのための比較演算子（fCostが小さい方が優先されるようにする）
	bool operator>(const Node& other) const
	{
		if (fCost != other.fCost) return fCost > other.fCost;
		if (gCost != other.gCost) return gCost > other.gCost;
		// 方向優先度（例：Up > Right > Down > Left）
		return static_cast<int>(fromDir) > static_cast<int>(other.fromDir);
	}
};

enum Environment
{
	高台,	// 高低差無視攻撃が可能なキャラ
	細道,	// 直線長距離攻撃が可能なキャラ
	壁際,	// 壁貫通攻撃が可能なキャラ
};

struct ActPattern_Attaker
{
	float MoveToAdvantage = 1.0f;	//有利ポジに向かう
	float MoveToEnemy = 1.0f;		//敵に向かう
	float Skill = 1.0f;				//スキル
	float AdvancedSkill = 1.0f;		//
	float UltimateSkill = 1.0f;		//
	int __MAX__ = 5;

	float GetHighestPriority() const
	{
		float maxPriority = 0;
		if (maxPriority < MoveToAdvantage)	maxPriority = MoveToAdvantage;
		if (maxPriority < MoveToEnemy)		maxPriority = MoveToEnemy;
		if (maxPriority < Skill)			maxPriority = Skill;
		if (maxPriority < AdvancedSkill)	maxPriority = AdvancedSkill;
		if (maxPriority < UltimateSkill)	maxPriority = UltimateSkill;
		
		return maxPriority;
	}
};
enum class ActPattern_Buffer
{

};
enum class ActPattern_Healer
{

};

enum class SkillType
{
	Attack = 0,
	Heal,
	Buff,
};
struct SkillInfo
{
	std::string skillName;	// スキル識別タグ
	uint32_t cost; // 使用するために必要なコスト
	uint32_t delayCost; // 使用できない期間(必殺技のみ)

	uint32_t range;// 効果範囲

	SkillType skillType;	// 攻撃、回復、バフ、etc...スキルのタイプ
	float skillOdds = 1.0f;		// スキルそのものの倍率
	uint32_t skillLevel = 1;// ゲーム外で上げれるパラメータ。上げる程効果アップ。レベル＊効果量
	// スキル効果量(回復量、ダメージ量等) = キャラ攻撃力 * skillOdds * skillLevel * キャラバフ + 固定値バフ
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
	uint32_t delayCost; // 使用できない期間(必殺技のみ)

	uint32_t moveRange; //移動可能範囲

	uint32_t actionDelay; //（技を使用する度に技コスト分加算される。場にいるキャラでこの数字が最も少ないキャラが最初に行動する

	Environment advantagePosition; // 有利ポジション

	ActPattern_Attaker actPattern;	// 行動パターン 
	ActPattern_Attaker priority;	// 行動優先度初期値
	void priorityReset() { actPattern = priority; }

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

	void DecideAction(Charactor& self, const Charactor& enemy);

	bool translateBlock(float EasingMax, Transforms& transforms);

	// 座標からマップ上のインデックスを求める関数
	Vector2int PositionToIndex(Vector3 pos);
	// マップ上のインデックスから座標を求める関数
	Vector3 IndexToPosition(Vector2int index);

	bool CanMove(const Vector2int& from, const Vector2int& to) const;

	// A*変数初期化
	void AstarSet(const Vector3& pos, const Vector3& target);

	// Vector3→Vector3　最短経路構築　moveFromの更新あり
	void Astar(const Vector3& pos, const Vector3& target);
	// Vector3→Vector3　最短経路のマス数　moveFromの更新なし
	int GetShortestPathLength(const Vector3& pos, const Vector3& target);
	// Vector3→Vector3　最短経路のマスindex　moveFromの更新あり
	void GetShortestPosition(const Vector3& pos, const Vector3& target);
	// Environment→Vector3　最短経路のマス数　moveFromの更新なし
	int GetShortestPathLength(const Vector3& pos, const Environment& target);
	// Environment→Vector3　最短経路のマスindex　moveFromの更新あり
	void GetShortestPosition(const Vector3& pos, const Environment& target);

private:
	// ターン
	Turn turn;
	Turn turnRepuest;

	// 移動経路
	std::vector<Vector2int> pathNodes;
	int pathStepIndex = 0;
	int enemyMoveActCounter; // 敵のそのターンにおける移動回数
	// 移動先選択中
	Vector3 moveTarget;
	Vector2int moveFrom;
	// 移動中
	bool movement;
	float movementT;
	float movementTmax;
	// 移動先
	Vector2int nextIndex;
	Transforms nextBlockTransforms;
	// 移動元
	Vector2int fromIndex;
	Transforms fromBlockTransforms;
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
	int torusModel = Game::LoadOBJ("resources/Mesh", "torus.obj");



	//-----------------------------マップ編集----------------------------
	bool editMode;
	int selectBlock;
	bool PrePressMouse;

	int warpInBlockPng = Game::LoadTexture("resources/block/map_warpIn.png");
	int warpOutBlockPng = Game::LoadTexture("resources/block/map_warpOut.png");

	int emptyPng = Game::LoadTexture("resources/blockType/empty.png");
	int wallPng = Game::LoadTexture("resources/blockType/wall.png");
	int asidPng = Game::LoadTexture("resources/blockType/asid.png");
	int warpInPng = Game::LoadTexture("resources/blockType/warpIn.png");
	int warpOutPng = Game::LoadTexture("resources/blockType/warpOut.png");
	int SelectemptyPng = Game::LoadTexture("resources/blockType/Selectempty.png");
	int SelectwallPng = Game::LoadTexture("resources/blockType/Selectwall.png");
	int SelectasidPng = Game::LoadTexture("resources/blockType/Selectasid.png");
	int SelectwarpInPng = Game::LoadTexture("resources/blockType/SelectwarpIn.png");
	int SelectwarpOutPng = Game::LoadTexture("resources/blockType/SelectwarpOut.png");

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

