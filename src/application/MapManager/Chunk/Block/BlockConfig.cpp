#include "MapManager/Chunk/Block/BlockConfig.h"

BlockConfig::BlockConfig()
{
	// ブロック情報初期化
	//				BlockID			耐久値		透過ブロックか
	blockInfoMap_[BlockID::Air] = { BlockID::Air, -1, true };
	blockInfoMap_[BlockID::Stone] = { BlockID::Stone, 60.0f, false };
	blockInfoMap_[BlockID::Iron] = { BlockID::Iron, 800.0f, false };
	blockInfoMap_[BlockID::Diamond] = { BlockID::Diamond, 1200.0f, false };
	blockInfoMap_[BlockID::Bedrock] = { BlockID::Bedrock, std::numeric_limits<float>::infinity(), false };
	blockInfoMap_[BlockID::Dirt] = { BlockID::Dirt, 300.0f, false };
	blockInfoMap_[BlockID::Lawn] = { BlockID::Lawn, 300.0f, false };
	blockInfoMap_[BlockID::Glass] = { BlockID::Glass, 100.0f, true };
	blockInfoMap_[BlockID::Wood] = { BlockID::Wood, 400.0f , false };
	blockInfoMap_[BlockID::Leaf] = { BlockID::Leaf, 200.0f, true };
}

Blockinfo BlockConfig::GetBlockInfo(BlockID id) const
{
	return blockInfoMap_.at(id);
}
