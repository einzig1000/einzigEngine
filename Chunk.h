#pragma once
#include "Game.h"
#include <map>

class Block;

class Chunk
{
public:
	Chunk();
	~Chunk();

	std::unique_ptr<Block> blocks[CHUNK_X][CHUNK_Z][CHUNK_Y];
	// チャンク座標
	Vector2int chunkPos;
	// ブロックごとの描画データ管理マップ		
	std::map<BlockID, std::unique_ptr<RenderData_Block>> blockData_;

	void SetExposedBlocks();
	void Update();
	void Draw();
	void SaveChunkData(const std::string& mapFilePath);
};

