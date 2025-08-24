#include "Charactor.h"
#include "Map.h"

int Charactor::getCharactorSum_ = 0;

Charactor::Charactor(TEXTURE name)
{
	// 3Dの駒
	data.model = uint32_t(name);
	data.texture = uint32_t(name);
	data.transforms.translate = PositionByIndex(Vector2int(0, 5));

	// 情報が全部乗ってる2Dシート
	dataSeat.texture = uint32_t(TEXTURE::Charactor_SeatMini);

	// ↑にのってる2Dの駒
	if (name == TEXTURE::Charactor_King  )dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_King);
	else if (name == TEXTURE::Charactor_Queen )dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_Queen);
	else if (name == TEXTURE::Charactor_Bishop)dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_Bishop);
	else if (name == TEXTURE::Charactor_Knight)dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_Knight);
	else if (name == TEXTURE::Charactor_Rook  )dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_Rook);
	else if (name == TEXTURE::Charactor_Pawn  )dataSeat_type.texture = uint32_t(TEXTURE::CharactorSprite_Pawn);
	
	serialNumber_ = getCharactorSum_;
	getCharactorSum_++;
}

Charactor::~Charactor()
{}

void Charactor::Update()
{

}
