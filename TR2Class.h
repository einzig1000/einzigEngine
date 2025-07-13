#pragma once
#include "Game.h"
#include <queue>
#include <algorithm>

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
	None,
	Frontier, // openListにある状態  （探索待ちのノード）
	Explored, // closedListにある状態（すでに最適な経路で処理済み）
	Path,     // 最短経路に含まれる状態
};

struct Block : public Game::DrawObject
{
	BlockType type = BlockType::Empty;	// 壁？
	int cost = 0;						// 歩行コストト
	bool mouseTarget = false;			// マウスのターゲットか否か
	bool changeFlag = false;			 // マップ編集時の連続変更防止用

	// AI行動決定時のターゲット表示用
	bool isAttackTarget = false;// 攻撃予定地;
	bool isBuffTarget = false;	// 回復予定地;
	bool isHealTarget = false;	// バフ予定地;

	AstarBlockState state = AstarBlockState::None; // A*アルゴリズムの視覚化用
};

enum class Turn
{
	None,			// 初期化
	Player,			// プレイヤー行動選択、行動実行がまとまってる(PlayerConsiderの追加があってもいい)
	EnemyConsider,  // エネミーが行動を考えるフェーズ
	Enemy,			// エネミー行動実行
};

enum class SkillType
{
	Attack,
	Buff,
	Heal,
};

struct SkillInfo
{
	std::string skillName;	// スキル識別タグ
	int cost;				// 使用するために必要なコスト
	int delayCost;			// 使用できない期間(必殺技のみ(多分))
	int range;				// 効果範囲(単体バフもあるだろ！！！)
	float skillOdds;		// スキルそのものの倍率
	SkillType skillType;	// 攻撃、回復、バフ、etc...スキルのタイプ
	uint32_t skillLevel = 1;// ゲーム外で上げれるパラメータ。上げる程効果アップ。レベル＊効果量
	// スキル効果量(回復量、ダメージ量等) = キャラ攻撃力 * skillOdds * skillLevel * キャラバフ + 固定値バフ
};

enum Environment
{
	高台,	// 高低差無視攻撃が可能なキャラ
	細道,	// 直線長距離攻撃が可能なキャラ
	壁際,	// 壁貫通攻撃が可能なキャラ
};

// AIの行動優先度
struct ActPattern_Attaker
{
	float MoveToAdvantage = 1.0f;	//有利ポジに向かう
	float MoveToEnemy = 1.0f;		//敵に向かう
	float Skill = 1.0f;				//スキル
	float AdvancedSkill = 1.0f;		//
	float UltimateSkill = 1.0f;		//

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
struct ActPattern_Buffer
{

};
struct ActPattern_Healer
{

};

struct Charactor : public Game::DrawObject
{
	int HP = 0;
	int Attack = 0;
	int moveRange = 0;
	uint32_t actionDelay; //（技を使用する度に技コスト分加算される。場にいるキャラでこの数字が最も少ないキャラが最初に行動する


	int skill = 0;			// 通常スキルインデックス
	int advancedSkill = 0;	// 強スキルインデックス
	int passiveSkill = 0;	// パッシブスキルインデックス
	int ultimateSkill = 0;	// ウルトスキルインデックス
	int delayCost = 0;		// ウルト使用制限（クールダウン）

	// AI用
	Environment advantagePosition = Environment::高台; // 自身にとっての有利ポジ
	ActPattern_Attaker priority; // 行動優先度初期値
	ActPattern_Attaker actPattern; // 実際の行動決定に使われる優先度

	void priorityReset()
	{
		actPattern = priority;
	}
};

// A*ノード構造体
struct Node
{
	Vector2int index;     // ノードの座標
	int gCost;            // 開始地点からの実コスト
	int hCost;            // 目標地点までの推定コスト (ヒューリスティック)
	int fCost;            // gCost + hCost + (ブロック固有のコスト)
	Vector2int parentIndex; // 親ノードの座標
	Direction fromDirection; // どの方向から来たか
	bool fromSlope;       // Slopeから来たか

	bool operator>(const Node& other) const
	{
		if (fCost != other.fCost) return fCost > other.fCost;
		if (gCost != other.gCost) return gCost > other.gCost;
		return false;
	}
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


class TR2Class
{
public:
	TR2Class();
	void Initialize();
	void Update();
	void Draw();



	//void Initialize_PlayerTurn();
	//void Initialize_EnemyConsiderTurn();
	//void Initialize_EnemyTurn();

	//void Update_PlayerTurn();
	//void Update_EnemyConsiderTurn();
	//void Update_EnemyTurn();

	//void Draw_PlayerTurn();
	//void Draw_EnemyConsiderTurn();
	//void Draw_EnemyTurn();

	//void DecideAction(Charactor& self, const Charactor& enemy);

	//bool translateBlock(float EasingMax, Transforms& transforms);

	//// 座標からマップ上のインデックスを求める関数
	//Vector2int PositionToIndex(Vector3 pos);
	//// マップ上のインデックスから座標を求める関数
	//Vector3 IndexToPosition(Vector2int index);

	//bool CanMove(const Vector2int& from, const Vector2int& to) const;

	//// A*変数初期化
	//void AstarSet(const Vector3& pos, const Vector3& target);

	//// Vector3→Vector3　最短経路構築　moveFromの更新あり
	//void Astar(const Vector3& pos, const Vector3& target);
	//// Vector3→Vector3　最短経路のマス数　moveFromの更新なし
	//int GetShortestPathLength(const Vector3& pos, const Vector3& target);
	//// Vector3→Vector3　最短経路のマスindex　moveFromの更新あり
	//void GetShortestPosition(const Vector3& pos, const Vector3& target);
	//// Environment→Vector3　最短経路のマス数　moveFromの更新なし
	//int GetShortestPathLength(const Vector3& pos, const Environment& target);
	//// Environment→Vector3　最短経路のマスindex　moveFromの更新あり
	//void GetShortestPosition(const Vector3& pos, const Environment& target);

private:
	// ====== モデル・テクスチャ ======
	uint32_t playerModel;
	uint32_t blockModel;
	uint32_t skyDomeModel;
	uint32_t blockParticleModel;

	uint32_t uvCheckerPng;
	uint32_t blockPng;
	uint32_t skyDomePng;
	uint32_t blockParticlePng;


	// UI用テクスチャ
	uint32_t emptyPng;
	uint32_t SelectemptyPng;
	uint32_t wallPng;
	uint32_t SelectwallPng;
	uint32_t asidPng;
	uint32_t SelectasidPng;
	uint32_t warpInPng;
	uint32_t SelectwarpInPng;
	uint32_t warpOutPng;
	uint32_t SelectwarpOutPng;


	// ====== ゲームオブジェクト ======
	Charactor player_;
	Charactor enemy_;
	Block block[MAP_HEIGHT][MAP_WIDTH];
	Game::DrawObject skyDome;


	// ====== ターン管理 ======
	Turn turn;
	Turn turnRepuest; // 次のターンへのリクエスト

	// ====== マップ編集 ======
	bool editMode;
	int selectBlock; // 選択中のブロックタイプ
	bool PrePressMouse; // 前フレームのマウスボタン状態

	// ====== 移動関連 ======
	std::vector<Vector2int> pathNodes; // 経路のノードリスト
	int pathStepIndex = 0; // 現在移動中の経路ステップ
	int enemyMoveActCounter; // 敵のそのターンにおける移動回数

	// プレイヤーが選択した移動目標地点
	Vector2int playerSelectedMoveTarget;

	// 実際の移動アニメーション用（補間元のTransformと補間先のTransform）
	Transforms fromBlockTransforms;
	Transforms nextBlockTransforms;

	// 移動中フラグとイージング用変数
	bool movement;
	float movementT;
	Direction direction; // 移動方向 (アニメーション用)

	// ====== A*パスファインディング ======
	std::vector<std::vector<int>> gCostMap; // スタート地点からの実コストマップ
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList; // 探索中のノード（優先度キュー）
	std::map<Vector2int, Vector2int> parentMap; // 経路再構築のための親ノードマップ

	Vector2int astarStartIndex; // グローバルA*探索の開始地点
	Vector2int astarTargetIndex; // グローバルA*探索の目標地点
	bool aStarActive; // グローバルA*探索がアクティブかどうかのフラグ

	// 敵のAIが決定した行動目標地点
	Vector2int enemyActionTarget;

	// ====== スキル情報 ======
	std::vector<SkillInfo> skillList;
	std::vector<SkillInfo> advancedSkillList;
	std::vector<SkillInfo> ultimateSkillList;


private:
	// ====== 初期化・更新・描画関数 ======
	void Initialize_PlayerTurn();
	void Update_PlayerTurn();
	void Draw_PlayerTurn();

	void Initialize_EnemyConsiderTurn();
	void Update_EnemyConsiderTurn();
	void Draw_EnemyConsiderTurn();

	void Initialize_EnemyTurn();
	void Update_EnemyTurn();
	void Draw_EnemyTurn();

	// ====== ヘルパー関数 ======
	// マップ座標変換
	Vector2int PositionToIndex(Vector3 pos);
	Vector3 IndexToPosition(Vector2int index);

	// 移動アニメーション制御(移動中はfalseを返すぜ！)
	bool translateBlock(float easingDuration, Transforms& characterTransforms);

	// マップ編集モードの更新
	void _UpdateMapEditMode();


	// ====== A*パスファインディング関連 ======
	// A*のコアロジックをまとめたプライベートヘルパー関数
	// outParentMap: 経路再構築のための親ノードマップ
	// outGCostMap: Gコストを格納するマップ
	// outPathNodes: パスノードリスト (nullptrの場合はパスを構築しない)
	// 戻り値: パスが見つかったかどうか
	bool _RunAstar(
		const Vector2int& start,
		const Vector2int& target,
		std::map<Vector2int, Vector2int>& outParentMap,
		std::vector<std::vector<int>>& outGCostMap,
		std::vector<Vector2int>* outPathNodes = nullptr
	);

	// 実際の移動パスの計算と可視化を行うA*
	void Astar(const Vector3& pos, const Vector3& target);

	// 2点間の最短経路長を計算するA* (マップ状態を変更しない)
	int GetShortestPathLength(const Vector3& pos, const Vector3& target);

	// 特定の環境タイプ（例:高台）までの最短経路長を計算するA*
	int GetShortestPathLength(const Vector3& pos, const Environment& targetType);

	// 特定の目標地点（Vector3）を敵の行動目標として設定 (GetShortestPositionの役割はAIのターゲット設定に移譲)
	void SetEnemyActionTarget(const Vector3& target);

	// 特定の環境タイプ（例:高台）の中で、posから最も近い地点を敵の行動目標として設定
	void SetEnemyActionTarget(const Vector3& pos, const Environment& targetType);


	// 2点間の移動がゲームルール上可能か判定
	bool CanMove(const Vector2int& from, const Vector2int& to) const;

	// ====== AI ======
	// 敵の行動を決定
	void DecideAction(Charactor& self, const Charactor& enemy);
};

//	// 移動経路
//	std::vector<Vector2int> pathNodes;
//	int pathStepIndex = 0;
//	int enemyMoveActCounter; // 敵のそのターンにおける移動回数
//	// 移動先選択中
//	Vector3 moveTarget;
//	Vector2int moveFrom;
//	// 移動中
//	bool movement;
//	float movementT;
//	float movementTmax;
//	// 移動先
//	Vector2int nextIndex;
//	Transforms nextBlockTransforms;
//	// 移動元
//	Vector2int fromIndex;
//	Transforms fromBlockTransforms;
//	// 移動方向
//	Direction direction;
//
//
//
//	// プレイヤー
//	Charactor player_;
//
//	// エネミー
//	Charactor enemy_;
//
//	// ブロック
//	Block block[MAP_HEIGHT][MAP_WIDTH];
//	
//
//	// 天球
//	Transforms skyDomeTransforms;
//	DrawOptions skyDomeOptions;
//
//
//	//---------------------------model&texture------------------------
//	int uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
//	int playerModel = Game::LoadOBJ("resources/player", "player.obj");
//	int blockModel = Game::LoadOBJ("resources/block", "map.obj");
//	int blockPng = Game::LoadTexture("resources/block/map.png");
//	int skyDomeModel = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
//	int skyDomePng = Game::LoadTexture("resources/skyDome/skyDome.png");
//	int torusModel = Game::LoadOBJ("resources/Mesh", "torus.obj");
//
//
//
//	//-----------------------------マップ編集----------------------------
//	bool editMode;
//	int selectBlock;
//	bool PrePressMouse;
//
//	int warpInBlockPng = Game::LoadTexture("resources/block/map_warpIn.png");
//	int warpOutBlockPng = Game::LoadTexture("resources/block/map_warpOut.png");
//
//	int emptyPng = Game::LoadTexture("resources/blockType/empty.png");
//	int wallPng = Game::LoadTexture("resources/blockType/wall.png");
//	int asidPng = Game::LoadTexture("resources/blockType/asid.png");
//	int warpInPng = Game::LoadTexture("resources/blockType/warpIn.png");
//	int warpOutPng = Game::LoadTexture("resources/blockType/warpOut.png");
//	int SelectemptyPng = Game::LoadTexture("resources/blockType/Selectempty.png");
//	int SelectwallPng = Game::LoadTexture("resources/blockType/Selectwall.png");
//	int SelectasidPng = Game::LoadTexture("resources/blockType/Selectasid.png");
//	int SelectwarpInPng = Game::LoadTexture("resources/blockType/SelectwarpIn.png");
//	int SelectwarpOutPng = Game::LoadTexture("resources/blockType/SelectwarpOut.png");
//
//	//------------------------------skill----------------------------
//	std::vector<SkillInfo>skillList;
//	std::vector<SkillInfo>advancedSkillList;
//	std::vector<SkillInfo>passiveSkillList;
//	std::vector<SkillInfo>ultimateSkillList;
//
//
//
//	//-------------------------------A*-----------------------------
//
//
//	// A*アルゴリズムに必要なデータ構造
//	// 1. オープンリスト（これから評価する可能性のあるマスのリスト。fが最も小さいマスが優先）
//	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
//
//	// 2. 各ノードのgCostを保持（訪問済みかどうかの確認と、より良い経路が見つかったかの判断
//	std::vector<std::vector<int>> gCostMap;
//
//	// 3. 経路再構築のための親ノードのマップ
//	std::map<Vector2int, Vector2int> parentMap;
//
//	// A*スタートとゴール
//	Vector2int startIndex;
//	Vector2int targetIndex;
//	bool aStarActive;
//
//
//
//};
//
