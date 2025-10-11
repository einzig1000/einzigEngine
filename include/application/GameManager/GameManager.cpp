#include "GameManager.h"

GameManager::GameManager()
{
	frame = 0;
	uint32_t tex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");

	sprite.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	ground.model = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");
	ground.texture = tex;
	groun.model = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");
	groun.texture = tex;
	player.model = Game::LoadOBJ("resources/Minecraft/blaze/", "blaze.obj");
	player.model = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");
	player.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");

	player.transforms.translate = { 0.0f,10.0f,0.0f };
	player.gravity.y = 0.01f;

	ground.transforms.scale = { 10.0f,1.0f,10.0f };
	groun.transforms.translate = { 0.0f,-1.0f,0.0f };

	player.SetBlock(ground);
	//player.SetBlock(ground);
	player.SetBlock(groun);
	//ground.SetBlock(player);
}

GameManager::~GameManager()
{

}

void GameManager::Update()
{
	if (frame > 1)
	{
		// ジャンプ
		if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
		{
			player.velocity.y += 0.3f;
			player.gravity.y = 0.01f;
		}
		if (GetPadState::buttons[0][PAD_A] && !GetPadState::preButtons[0][PAD_A])
		{
			player.velocity.y += 0.3f;
			player.gravity.y = 0.01f;
		}

		//if (GetHitKey::keys[DIK_W] || GetHitKey::keys[DIK_S])
		//{
		//	if (GetHitKey::keys[DIK_W])
		//		player.velocity.z = 0.03f;
		//	if (GetHitKey::keys[DIK_S])
		//		player.velocity.z = -0.03f;
		//}
		//else player.velocity.z = 0.0f;
		//
		//if (GetHitKey::keys[DIK_D] || GetHitKey::keys[DIK_A])
		//{
		//	if (GetHitKey::keys[DIK_D])
		//		player.velocity.x = 0.03f;
		//	if (GetHitKey::keys[DIK_A])
		//		player.velocity.x = -0.03f;
		//}
		//else player.velocity.x = 0.0f;

		// 
		if (GetHitKey::keys[DIK_1])
		{
			player.LookAtFront();
		}
		if (GetHitKey::keys[DIK_2])
		{
			player.LookAtCamera(roll);
		}

		if (GetPadState::buttons[0][PAD_DOWN])
		{
			player.transforms.translate.z -= 0.1f;
		}
		if (GetPadState::buttons[0][PAD_UP])
		{
			player.transforms.translate.z += 0.1f;
		}
		if (GetPadState::buttons[0][PAD_RIGHT])
		{
			player.transforms.translate.x += 0.1f;
		}
		if (GetPadState::buttons[0][PAD_LEFT])
		{
			player.transforms.translate.x -= 0.1f;
		}

		// 
		if (GetPadState::rightStickDir[0].x > 0.25f)
		{
			cameraRotate.y += 0.1f;
		}
		else if (GetPadState::rightStickDir[0].x < -0.25f)
		{
			cameraRotate.y -= 0.1f;
		}
		if (GetPadState::rightStickDir[0].y > 0.25f)
		{
			cameraRotate.x -= 0.03f;
		}
		else if (GetPadState::rightStickDir[0].y < -0.25f)
		{
			cameraRotate.x += 0.03f;
		}

	}

	Game::MoveCameraRotate(cameraRotate, 0, EaseType::IN_BACK);
	
	roll += 0.01f;
	frame++;
}

void GameManager::Draw()
{
	//Game::DrawLine(Vector3{ 0,0,0 }, Vector3{ 100,100,100 }, 0xFFFFFFFF);
	//sprite.Draw();
	player.Draw();
	//player.DrawAABB();
	//player.DrawImGui();
	ground.Draw();
	//ground.DrawAABB();
	//ground.DrawImGui();
	//groun.Draw();
	//groun.DrawAABB();
	//groun.DrawImGui();
}