#pragma once
#include "Game.h"
#include <map>
#include "PerlinNoise.h"

class Block;
class BlockConfig;

class Chunk
{
public:
	Chunk();
	~Chunk();

	// チャンクデータ生成
	void CreateChunkData(const NoiseParameter& param, const Vector2int& chunkPos);
	void CreateChunkDataFromJson();		// Jsonからチャンクデータを読み込み
	void CreateChunkDataNewly(const NoiseParameter& param, const Vector2int& chunkPos);	// 新規生成チャンクデータ作成
	void GenerateOres(const NoiseParameter& param);	// 鉱石を生成
	void GenerateTrees(const NoiseParameter& param);// 木を生成

	void SetNeighborChunk(int direction, Chunk* neighbor);
	bool IsNeighborExist(int direction);
	void Update();
	void Draw();

	// ブロック取得  自チャンク＋隣接チャンク対応 (隣接チャンクは隣接しているブロックのみ)
	Block* GetBlock(const Vector3int& localIndex);

	// AABB取得  全チャンク対応
	AABB GetAABB(const Vector3int& index);

	// 座標取得  
	Vector3 LocalCenter(const Vector3int& index) const;

	// ブロック設置(置換)
	void SetBlockLocal(const Vector3int& index, const BlockID id);

	// ブロック破壊
	void DestroyBlock(const Vector3int& localIndex);

	// blockPositionsの再構築
	void RebuildBlockPositions();

	// インスタンスを作成する
	void CreateInstance();

	// 表面に露出しているブロックを判定
	void SetExposedBlocks();

	// localIndexの周り６ブロックの露出状態を更新
	void UpdateExposedAround(const Vector3int& localIndex);


	// Jsonから読み込まれていたか(初めての生成かどうか)
	// true : 既にマップのセーブデータに存在していたチャンク
	// false : 新規生成されたチャンク
	bool loadResult = false;

	
	// チャンク座標
	Vector2int chunkPos;
	// ブロックごとの位置リスト
	std::unordered_map<BlockID, std::vector<Vector3int>> blockPositions;
	// ブロックデータ配列
	std::unique_ptr<Block> blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	// ブロックごとの描画データ管理マップ		
	std::map<BlockID, std::unique_ptr<RenderData_Block>> blockData_;


	Chunk* neighbors[4] = { nullptr, nullptr, nullptr, nullptr }; // 0:+X,1:-X,2:+Z,3:-Z

	BlockConfig* blockConfig_;
};

