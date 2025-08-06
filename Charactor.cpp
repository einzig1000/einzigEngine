#include "Charactor.h"
#include "Map.h"

int Charactor::getCharactorSum_ = 0;

Charactor::Charactor(TEXTURE name)
{
	data.model = uint32_t(name);
	data.texture = uint32_t(name);
	data.transforms.translate = PositionByIndex(Vector2int(0, 5));

	serialNumber_ = getCharactorSum_;
	getCharactorSum_++;
}

Charactor::~Charactor()
{}

void Charactor::Update()
{

}
