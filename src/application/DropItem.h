#pragma once
#include "Game.h"

class Player;

class DropItem
{
public:

	//
	// DropItemもRenderData_Blockで描画
	// 






	DropItem(Player* player, Vector3int indec, Vector3 pos, int model, int tex);
	void Update(bool isUnderBlock);
	void Draw();

	bool isDestroy_ = false;

	RenderData_Model Item;

	Vector3int index;
	Vector3 offsetPos = { 0.0f,0.0f,0.0f };
	float Yoffset = 0.0f;
	Vector3 InitPos = { 0.0f,0.0f,0.0f };
	int frame;

	Player* player_;

	Vector3int IndexByPosition(const Vector3& position)
	{
		Vector3int index;
		index.x = static_cast<int>(((position.x + (CHUNK_X - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));
		index.y = static_cast<int>(((position.y + (CHUNK_Z - 1)) / BLOCK_SIZE));
		index.z = static_cast<int>(((position.z + (CHUNK_Y - 1)) / BLOCK_SIZE) + (BLOCK_SIZE / 2.0f));

		if (index.x < 0)index.x = 0;
		else if (index.x > CHUNK_X - 1)index.x = CHUNK_X - 1;
		if (index.y < 0)index.y = 0;
		else if (index.y > CHUNK_Z - 1)index.y = CHUNK_Z - 1;
		if (index.z < 0)index.z = 0;
		else if (index.z > CHUNK_Y - 1)index.z = CHUNK_Y - 1;

		return index;
	}

};

