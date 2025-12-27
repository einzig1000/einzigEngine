#include "Block/BlockConfig.h"

BlockConfig::BlockConfig()
{
	blockInfoMap_[BlockID::Air] = { BlockID::Air, -1 };
	blockInfoMap_[BlockID::Stone] = { BlockID::Stone, 600 };
	blockInfoMap_[BlockID::Dirt] = { BlockID::Dirt, 300 };
	blockInfoMap_[BlockID::Lawn] = { BlockID::Lawn, 300 };
	blockInfoMap_[BlockID::Glass] = { BlockID::Glass, 100 };
	blockInfoMap_[BlockID::Wood] = { BlockID::Wood, 400 };
	blockInfoMap_[BlockID::Leaf] = { BlockID::Leaf, 200 };
}

Blockinfo BlockConfig::GetBlockInfo(BlockID id) const
{
	return blockInfoMap_.at(id);
}
