#include "MapManager/DropItem/DropItem.h"
#include "Charactor/Player/Player.h"

DropItem::DropItem(Player* player, Vector3int indec, Vector3 pos, int model, int tex)
{
	Item.SetModel(model);
	Item.SetTexture(tex);
	Item.scale.value = Vector3(0.2f, 0.2f, 0.2f);
	InitPos = pos;
	index = indec;
	Yoffset = 0.0f;
	player_ = player;
	isDestroy_ = false;

	frame = 0;
}

void DropItem::Update(bool isUnderBlock)
{
	if (isDestroy_) return;
	frame++;

	offsetPos.y = sinf(frame / 20.0f) * 0.2f;
	Item.translate.value = InitPos + offsetPos;
	Item.translate.value.y += Yoffset;
	Item.rotate.value.y += 0.04f;

	if (isUnderBlock)
	{
		Yoffset -= 0.05f;
	}
	if (frame > 60)
	{
		//if (IsCollision(player_->data_.aabbs[0], Item.aabbs[0]) && !isDestroy_)
		//{
		//	isDestroy_ = true;
		//	player_->AddItemToItemslot(Item.texture);
		//}
	}



	index = IndexByPosition(Item.translate.value);
}

void DropItem::Draw()
{
	if (!isDestroy_)
	Item.Draw();
}
