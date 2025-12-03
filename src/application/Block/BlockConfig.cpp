#include "Block/BlockConfig.h"

BlockConfig::BlockConfig()
{
	blockInfoMap_[BlockID::Air] = { BlockID::Air, -1 };
	blockInfoMap_[BlockID::Stone] = { BlockID::Stone, 60 };
	blockInfoMap_[BlockID::Dirt] = { BlockID::Dirt, 30 };
	blockInfoMap_[BlockID::Lawn] = { BlockID::Lawn, 30 };
	blockInfoMap_[BlockID::Glass] = { BlockID::Glass, 10 };
	blockInfoMap_[BlockID::Wood] = { BlockID::Wood, 40 };
	blockInfoMap_[BlockID::Leaf] = { BlockID::Leaf, 20 };
}

Blockinfo BlockConfig::GetBlockInfo(BlockID id) const
{
	return blockInfoMap_.at(id);
}
