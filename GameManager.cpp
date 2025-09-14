#include "GameManager.h"

GameManager::GameManager()
{
	frame = 0;

	test.model = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");
	test.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	test1.model = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");
	test1.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");

	test.transforms.translate = { 0.0f,10.0f,0.0f };
	test.gravity = 0.01f;

	test1.transforms.scale = { 10.0f,1.0f,10.0f };
}

GameManager::~GameManager()
{

}

void GameManager::Update()
{
	if (frame > 1)
	{
		if (test.isCollision(test1))
		{
			test.velocity.y = 0.0f;
			test.gravity = 0.0f;
		}
		if (!GetHitKey::preKeys[DIK_SPACE])
		{
			if (GetHitKey::keys[DIK_SPACE])
			{
				test.velocity.y += 5.0f;
			}
		}

		if (GetHitKey::keys[DIK_1])
		{
			test.LookAtFront();
		}
		if (GetHitKey::keys[DIK_2])
		{
			test.LookAtCamera();
		}
	}


	frame++;
}

void GameManager::Draw()
{
	test.Draw();
	test.DrawAABB();
	test.DrawImGui();
	test1.Draw();
	test1.DrawAABB();
	test1.DrawImGui();
}