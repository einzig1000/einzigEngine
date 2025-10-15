#include "PlayerBullet.h"


PlayerBullet::PlayerBullet()
{
	data.model = Game::LoadOBJ("resources/Prototypes/model/", "sphere.obj");
	data.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	data.transforms.scale = { 0.01f,0.01f,0.01f };
}

bool PlayerBullet::Update(Vector3 pos, bool action, float radius)
{
	data.transforms.translate = pos;
	data.transforms.translate.y = -1.0f;

	if (action)
	{
    	data.transforms.scale += { radius / 20.0f, radius / 20.0f, radius / 20.0f };
		// data.transforms.scale.x >= radiusになると同時にdolorの透明度が0になるように調整
		data.color -= 12;

		if (data.transforms.scale.x >= radius)
		{
			data.transforms.scale = { radius,radius,radius };
			return true;
		}
	}
	else
	{
		data.color = 0xFFFFFFFF;
		data.transforms.scale = { 0.01f,0.01f,0.01f };
	}

	return false;
}

void PlayerBullet::Draw()
{
	data.Draw();
}