#include "GameScenePhase.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z].translate.value = Vector3(
					x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
					y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
					z * BLOCK_SIZE - (MAX_BLOCK_Z - 1)
				);
			}
		}
	}

	//Game::Input::Mouse::ToggleMouseCursorVisible();

	//cameraPos = Game::Camera::Getter::GetCurrentCenter();
	
	LoadMap("resources/Map/map.csv");
}

GameScenePhase::~GameScenePhase()
{

}

void GameScenePhase::Initialize()
{
	nextPhase = PHASE::Phase_None;
}


void GameScenePhase::Update()
{
	UpdateMousePos();
	UpdateCamera();

	//Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::IN_BACK);
	//Game::Camera::MoveCameraRotate(cameraRot, 0, EaseType::IN_BACK);
}


void GameScenePhase::Draw()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z].Draw();
			}
		}
	}
}


void GameScenePhase::LoadMap(const std::string& mapFilePath)
{
	std::ifstream file(mapFilePath);
	// ファイルがなかったら0埋めでファイルを作成
	if (!file.is_open())
	{
		std::ofstream createFile(mapFilePath);
		for (int z = 0; z < MAX_BLOCK_Z; z++)
		{
			for (int y = 0; y < MAX_BLOCK_Y; y++)
			{
				for (int x = 0; x < MAX_BLOCK_X; x++)
				{
					createFile << "3";
					if (x < MAX_BLOCK_X - 1)
					{
						createFile << ",";
					}
				}
				createFile << "\n";
			}
		}
		createFile.close();
		file.open(mapFilePath);
	}

	std::string line;
	std::getline(file, line);
	int X = 0;
	int Y = 0;
	int Z = 0;

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string BlockID;
		while (std::getline(ss, BlockID, ','))
		{
			if (X < MAX_BLOCK_X && Y < MAX_BLOCK_Y && Z < MAX_BLOCK_Z)
			{
				// 空気
				if (BlockID == "0")
				{
					block_[X][Y][Z].texture = -1;
					block_[X][Y][Z].model =
						Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
					block_[X][Y][Z].options.wireframe = true;
				}
				else if (BlockID == "1")
				{
					block_[X][Y][Z].texture =
						Game::Resource::LoadTexture("resources/Minecraft/Stone.png");
					block_[X][Y][Z].model =
						Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
				}
				else if (BlockID == "2")
				{
					block_[X][Y][Z].texture =
						Game::Resource::LoadTexture("resources/Minecraft/Dirt.png");
					block_[X][Y][Z].model =
						Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
				}
				else if (BlockID == "3")
				{
					block_[X][Y][Z].texture =
						Game::Resource::LoadTexture("resources/Minecraft/Grass.png");
					block_[X][Y][Z].model =
						Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
				}
				else
				{
					block_[X][Y][Z].texture =
						std::stoi(BlockID);
				}

				X++;
				if (X >= MAX_BLOCK_X)
				{
					X = 0;
					Y++;
					if (Y >= MAX_BLOCK_Y)
					{
						Y = 0;
						Z++;
					}
				}
			}
		}
	}
}

void GameScenePhase::UpdateMousePos()
{
	preMousePos = mousePos;
	mousePos = Game::Input::Mouse::GetMousePosition();
	mouseGap = mousePos - preMousePos;
}

void GameScenePhase::UpdateCamera()
{
	// マウスで視点移動
	cameraRot.y += mouseGap.x * 0.01f;
	cameraRot.x += mouseGap.y * 0.01f;
	// 上下の回転制限
	if (cameraRot.x > 89.0f)
	{
		cameraRot.x = 89.0f;
	}
	if (cameraRot.x < -89.0f)
	{
		cameraRot.x = -89.0f;
	}
	// キーボードで移動
	Vector3 forward = {
		cosf(cameraRot.y) * cosf(cameraRot.x),
		sinf(-cameraRot.x),
		sinf(cameraRot.y) * cosf(cameraRot.x)
	};
	forward.Normalize();
	Vector3 right = {
		cosf(cameraRot.y + 90.0f),
		0.0f,
		sinf(cameraRot.y + 90.0f)
	};
	right.Normalize();
	if (Game::Input::Key::IsHeld(DIK_W))
	{
		cameraPos += forward * PLAYER_SPEED;
	}
	if (Game::Input::Key::IsHeld(DIK_S))
	{
		cameraPos -= forward * PLAYER_SPEED;
	}
	if (Game::Input::Key::IsHeld(DIK_A))
	{
		cameraPos -= right * PLAYER_SPEED;
	}
	if (Game::Input::Key::IsHeld(DIK_D))
	{
		cameraPos += right * PLAYER_SPEED;
	}
}