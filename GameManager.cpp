#include "GameManager.h"

GameManager::GameManager()
{
	test.model = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");
	test.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	test1.model = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");
	test1.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
}

GameManager::~GameManager()
{

}

void GameManager::Update()
{
	if (test.isCollision(test1))
	{
		test.color = 0xFF0000FF;
	}
	else
	{
		test.color = 0xFFFFFFFF;
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

void GameManager::Draw()
{
	test.Draw();
	test.DrawAABB();
	test.DrawImGui();
	test1.Draw();
	test1.DrawAABB();
	test1.DrawImGui();
}