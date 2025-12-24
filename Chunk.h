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
	void CreateChunkData(const NoiseParameter& param, const Vector2int& chunkPos);
	void Update();
	void Draw();


	AABB GetAABB(const Vector3int& index);

	// インスタンスを作成する
	void CreateInstance();

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

	// 表面に露出しているブロックを判定
	void SetExposedBlocks();



	BlockConfig* blockConfig_;
};

