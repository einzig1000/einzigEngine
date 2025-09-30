#include "GameManager.h"

GameManager::GameManager()
{
	frame = 0;

	player.model = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");
	player.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	ground.model = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");
	ground.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");

	player.transforms.translate = { 0.0f,10.0f,0.0f };
	player.gravity = 0.01f;

	ground.transforms.scale = { 10.0f,1.0f,10.0f };

	player.SetBlock(ground);
}

GameManager::~GameManager()
{

}

void GameManager::Update()
{
	if (frame > 1)
	{
		if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
		{
			player.velocity.y += 1.0f;
			player.gravity = 0.01f;
		}

		if (GetHitKey::keys[DIK_1])
		{
			player.LookAtFront();
		}
		if (GetHitKey::keys[DIK_2])
		{
			player.LookAtCamera(roll);
		}
	}

	roll += 0.01f;
	frame++;
}

void GameManager::Draw()
{
	player.Draw();
	player.DrawAABB();
	player.DrawImGui();
	ground.Draw();
	ground.DrawAABB();
	ground.DrawImGui();
}