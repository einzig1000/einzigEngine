#include "CG2_05.h"
#include "Game.h"
#include "functions.h"

CG2_05::CG2_05()
{
	uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	blockPng = Game::LoadTexture("resources/map.png");

	playerModel = Game::LoadOBJ("resources/model", "player.obj");
	blockModel = Game::LoadOBJ("resources/model", "map.obj");

	editMode = 0;


	Initialize();
}

void CG2_05::Initialize()
{
	// ブロック
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].Transforms.scale = { 1.0f, 1.0f, 1.0f };
			block[y][x].Transforms.translate = { -1.2f * x,0.0f,-1.2f * y };
			block[y][x].Transforms.rotate = { 0.0f,0.0f,0.0f };
			block[y][x].Color = 0xFFFFFFFF;
			block[y][x].AABB = CreateAABB(block[y][x].Transforms);
			block[y][x].changeFlag = 0;
			block[y][x].cost = 0;
			block[y][x].Type = Empty;
			block[y][x].mouseTraget = 0;
		}
	}

	// プレイヤー
	playerTransforms.scale = { 1.2f, 1.2f, 1.2f };
	playerTransforms.translate = block[MAP_HEIGHT - 1][MAP_WIDTH - 1].Transforms.translate;
	playerTransforms.rotate = { 0.0f,0.0f,0.0f };
	playerColor = 0xFFFFFFFF;
	playerAABB = CreateAABB(playerTransforms);

	// 敵
	enemyTransforms.scale = { 0.5f, 0.5f, 0.5f };
	enemyTransforms.translate = block[0][0].Transforms.translate;
	enemyTransforms.rotate = { 0.0f,0.0f,0.0f };
	enemyrColor = 0xFFFFFFFF;
	enemyAABB = CreateAABB(enemyTransforms);
	targetBlosk = { 0,0 };

	// カメライージングターゲット
	cameraRotate = { 0.82f, 0.0f, 0.0f };
	cameraCenter = { -5.44f, 0.0f, -5.0f };
	cameraDistance = 17;

	// マウス
	mousePos = { 0,0 };
}

void CG2_05::Update()
{
	// カメライージング
	if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
	{
		Game::MoveCenterTarget(cameraCenter, 120);
	}
	if (GetHitKey::keys[DIK_W] && !GetHitKey::preKeys[DIK_W])
	{
		Game::MoveRotateTarget(cameraRotate, 120);
	}
	if (GetHitKey::keys[DIK_E] && !GetHitKey::preKeys[DIK_E])
	{
		Game::MoveDistanceTarget(cameraDistance, 120);
	}
	if (GetHitKey::keys[DIK_A] && !GetHitKey::preKeys[DIK_A])
	{
		enemyTransforms.translate = block[targetBlosk.y][targetBlosk.x].Transforms.translate;
	}
	if (GetHitKey::keys[DIK_R] && !GetHitKey::preKeys[DIK_R])
	{
		Initialize();
	}

	// AABB作成
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].AABB = CreateAABB(block[y][x].Transforms);
		}
	}
	playerAABB = CreateAABB(playerTransforms);
	enemyAABB = CreateAABB(enemyTransforms);

	// マップ操作
	if (editMode)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (Game::IsCollisionMouseRayAABB(block[y][x].AABB, blockModel))
				{
					block[y][x].mouseTraget = 1;
				}
				else
				{
					block[y][x].mouseTraget = 0;
				}
			}
		}

		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].mouseTraget && Game::IsPressMouse(0))
				{
					if (block[y][x].Type == Empty && block[y][x].changeFlag == 0)
					{
						block[y][x].changeFlag = 1;
						block[y][x].Type = Wall;
						block[y][x].Transforms.scale.y = 5.0f;
						block[y][x].Transforms.translate.y = 0.5f;
					}
					else if (block[y][x].Type == Wall && block[y][x].changeFlag == 0)
					{
						block[y][x].changeFlag = 1;
						block[y][x].Type = Empty;
						block[y][x].Transforms.scale.y = 1.0f;
						block[y][x].Transforms.translate.y = 0.0f;
					}
				}
			}
		}

		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].mouseTraget == 1)
				{
					block[y][x].Color = 0xece99eFF;
				}
				else if (block[y][x].Type == Empty)
				{
					block[y][x].Color = 0xFFFFFFFF;
				}
				else if (block[y][x].Type == Wall)
				{
					block[y][x].Color = 0xFF005FFF;
				}
			}
		}

		if (!Game::IsPressMouse(0) && PrePressMouse)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					block[y][x].changeFlag = 0;
				}

			}
		}
	}



}

void CG2_05::Draw()
{
	// ブロック
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			Game::Drawobj(block[y][x].Transforms, block[y][x].Transforms.translate, blockModel, blockPng, block[y][x].Color);
		}
	}

	// プレイヤー
	Game::Drawobj(playerTransforms, { 0,0,0 }, playerModel, uvCheckerPng, enemyrColor);

	// 敵
	Game::Drawobj(enemyTransforms, { 0,0,0 }, playerModel, uvCheckerPng, enemyrColor);


	ImGui::Checkbox("mapEditMode", &editMode);

	if (ImGui::CollapsingHeader("blockTransforms"))
	{
		ImGui::DragFloat3("blockTransformsScale", &block[MAP_HEIGHT - 1][MAP_WIDTH - 1].Transforms.scale.x, 0.01f);
		ImGui::DragFloat3("blockTransformsRotate", &block[MAP_HEIGHT - 1][MAP_WIDTH - 1].Transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("blockTransformsTranslate", &block[MAP_HEIGHT - 1][MAP_WIDTH - 1].Transforms.translate.x, 0.01f);
	}
	if (ImGui::CollapsingHeader("enemyTransforms"))
	{
		ImGui::DragFloat3("enemyTransformsScale", &enemyTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("enemyTransformsRotate", &enemyTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("enemyTransformsTranslate", &enemyTransforms.translate.x, 0.01f);
		ImGui::DragInt2("targetBlosk", &targetBlosk.x);
	}
	if (ImGui::CollapsingHeader("playerTransforms"))
	{
		ImGui::DragFloat3("playerTransformsScale", &playerTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("playerTransformsRotate", &playerTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("playerTransformsTranslate", &playerTransforms.translate.x, 0.01f);
	}
	if (ImGui::CollapsingHeader("cameraTarget"))
	{
		ImGui::DragFloat3("targetCenter", &cameraCenter.x, 0.01f);
		ImGui::DragFloat3("targetRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat("targetDistance", &cameraDistance, 0.01f);
	}
	PrePressMouse = Game::IsPressMouse(0);
}

void CG2_05::Astar(BlockType& map)
{
	//if(map[0][2] == 0)

}