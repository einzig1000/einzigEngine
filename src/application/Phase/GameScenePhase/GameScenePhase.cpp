#include "GameScenePhase.h"
#include "ResourceID.h"
#include "Window/WindowManager.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z] = new Block();
				block_[x][y][z]->Initialize(
					Vector3(
						x * BLOCK_SIZE - (MAX_BLOCK_X - 1),
						y * BLOCK_SIZE - (MAX_BLOCK_Y - 1),
						z * BLOCK_SIZE - (MAX_BLOCK_Z - 1))
				);
			}
		}
	}

	// block_1つを少し大きめに囲う形に三角形を配置
	AABB blockAABB;
	blockAABB.min = Vector3(-BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f), -BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f), -BLOCK_SIZE / 2.0f - (BLOCK_SIZE / 100.0f));
	blockAABB.max = Vector3(BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f), BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f), BLOCK_SIZE / 2.0f + (BLOCK_SIZE / 100.0f));

	blockRect_[0].pos1 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[0].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[0].pos3 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[0].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);

	blockRect_[1].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[1].pos2 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[1].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[1].pos4 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);

	blockRect_[2].pos1 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[2].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[2].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[2].pos4 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);

	blockRect_[3].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[3].pos2 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[3].pos3 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[3].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);

	blockRect_[4].pos1 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[4].pos2 = Vector3(blockAABB.min.x, blockAABB.max.y, blockAABB.min.z);
	blockRect_[4].pos3 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.max.z);
	blockRect_[4].pos4 = Vector3(blockAABB.max.x, blockAABB.max.y, blockAABB.min.z);

	blockRect_[5].pos1 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[5].pos2 = Vector3(blockAABB.max.x, blockAABB.min.y, blockAABB.max.z);
	blockRect_[5].pos3 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.min.z);
	blockRect_[5].pos4 = Vector3(blockAABB.min.x, blockAABB.min.y, blockAABB.max.z);


	for (int i = 0; i < 6; i++)
	{
		blockRect_[i].texture = ResourceID::blockTextureIDs_[int(BlockTextureID::BreakBlock_0)];
	}

	reticle_.texture = ResourceID::TextureIDs_[int(TextureID::UVChecker)];
	reticle_.transforms.scale = Vector3(0.02f, 0.02f, 0.02f);
	reticle_.transforms.translate = Vector3(
		WindowManager::winWidth_ / 2.0f,
		WindowManager::winHeight_ / 2.0f,
		0.0f
	);

	Game::Input::Mouse::ToggleMouseCursorVisible();

	cameraPos = Game::Camera::Getter::GetCurrentCenter();
	
	LoadMap("resources/Map/map.csv");
}

GameScenePhase::~GameScenePhase()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				delete block_[x][y][z];
				block_[x][y][z] = nullptr;
			}
		}
	}
}

void GameScenePhase::Initialize()
{
	nextPhase = PHASE::Phase_None;
}


void GameScenePhase::Update()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Update();
				if (block_[x][y][z]->isDestroy_)
				{
					block_[x][y][z]->model_.scale = { 0.0f,0.0f,0.0f };
				}
			}
		}
	}
	UpdateMousePos();
	UpdateCamera();
	UpdateCollisionCenterRay();
	UpdateBlockTriangleTransform();
}


void GameScenePhase::Draw()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				block_[x][y][z]->Draw();
			}
		}
	}

	if (isDestroy_)
	{
		for (int i = 0; i < 6; i++)
		{
			blockRect_[i].Draw();
		}
	}


	reticle_.Draw();
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
					createFile << "1";
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
					block_[X][Y][Z]->model_.texture = -1;
					block_[X][Y][Z]->model_.options.wireframe = true;
				}
				else if (BlockID == "1")
				{
					block_[X][Y][Z]->model_.texture =
						ResourceID::blockTextureIDs_[int(BlockTextureID::Stone)];
					block_[X][Y][Z]->maxDurability_ = 60;
				}
				else if (BlockID == "2")
				{
					block_[X][Y][Z]->model_.texture =
						ResourceID::blockTextureIDs_[int(BlockTextureID::Dirt)];
					block_[X][Y][Z]->maxDurability_ = 30;
				}
				else if (BlockID == "3")
				{
					block_[X][Y][Z]->model_.texture =
						ResourceID::blockTextureIDs_[int(BlockTextureID::Grass)];
					block_[X][Y][Z]->maxDurability_ = 5;
				}
				else
				{
					block_[X][Y][Z]->model_.texture =
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

void GameScenePhase::UpdateBlockTriangleTransform()
{
	for (int i = 0; i < 6; i++)
	{
		blockRect_[i].transforms = blockTriangleTransform_;
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
	if (!Game::Input::Key::IsHeld(DIK_SPACE))
	{
		cameraRot.x += mouseGap.y * mouseSensitivity_;
		cameraRot.y += mouseGap.x * mouseSensitivity_;
		cameraRot.x = std::clamp<float>(cameraRot.x, -1.0f, 1.0f);
	}

	Vector3 forward = DirectionFromYawPitch(cameraRot.y, 0.0f);
	Vector3 right = DirectionFromYawPitch(cameraRot.y + 1.5708f, 0.0f);

	ImGui::Begin("Camera Info");
	ImGui::Text("Mouse X: %.2f, Y: %.2f", mouseGap.x, mouseGap.y);
	ImGui::DragFloat("sensitivity", &mouseSensitivity_, 0.001f, 0.001f, 1.0f);
	ImGui::DragFloat3("CameraPos", &cameraPos.x, 0.1f);
	ImGui::DragFloat3("CameraRot", &cameraRot.x, 0.01f);
	ImGui::End();

	// 移動（フレーム独立）
	if (Game::Input::Key::IsHeld(DIK_W)) cameraPos += forward * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_S)) cameraPos -= forward * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_A)) cameraPos -= right * PLAYER_SPEED;
	if (Game::Input::Key::IsHeld(DIK_D)) cameraPos += right * PLAYER_SPEED;

	Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraRotate(cameraRot, 3, EaseType::LINEAR);
}

void GameScenePhase::UpdateCollisionCenterRay()
{
	for (int x = 0; x < MAX_BLOCK_X; x++)
	{
		for (int y = 0; y < MAX_BLOCK_Y; y++)
		{
			for (int z = 0; z < MAX_BLOCK_Z; z++)
			{
				if (block_[x][y][z]->model_.isCollisionMouseRay == 0)
				{
					block_[x][y][z]->model_.color = { 0xFF, 0xFF, 0xFF, 0xFF };
					if (Game::Input::Mouse::IsHeld(0))
					{
						isDestroy_ = true;
						block_[x][y][z]->destroyFrame_++;
						for (int i = 0; i < 6; i++)
						{
							// 0.0f ~ 1.0f
							float t = float(block_[x][y][z]->destroyFrame_) / float(block_[x][y][z]->maxDurability_);
							// 0.0f ~ 10.0f
							float percent = t * 10.0f;
							// 0.0f ~ 5.0f
							float frameF = percent / 2.0f;
							// int変換
							int frame = int(frameF);
							if (frame > 4)frame = 4;

							int tagetID = int(BlockTextureID::BreakBlock_0) + frame;
							blockRect_[i].texture = ResourceID::blockTextureIDs_[tagetID];
						}
						blockTriangleTransform_.rotate = block_[x][y][z]->model_.rotate.value;
						blockTriangleTransform_.scale = block_[x][y][z]->model_.scale.value;
						blockTriangleTransform_.translate = block_[x][y][z]->model_.translate.value;
					}
					else
					{
						for (int i = 0; i < 6; i++)
						{
							blockRect_[i].texture = ResourceID::blockTextureIDs_[size_t(BlockTextureID::BreakBlock_0)];
						}
						block_[x][y][z]->destroyFrame_ = 0;
						isDestroy_ = false;
					}
				}
				else
				{
					block_[x][y][z]->model_.color = { 0x77, 0x77, 0x77, 0xFF };
				}
			}
		}
	}
}


// yaw, pitch, roll はラジアン、roll はここでは未使用
Vector3 GameScenePhase::DirectionFromYawPitch(float yaw, float pitch)
{
	float sp = std::sinf(pitch);
	float cp = std::cosf(pitch);
	float sy = std::sinf(yaw);
	float cy = std::cosf(yaw);

	Vector3 dir;
	dir.x = sy * cp;
	dir.y = -sp;
	dir.z = cy * cp;
	dir.Normalize();
	return dir;
}

Vector3 GameScenePhase::YawPitchFromDirection(const Vector3& dir)
{
	Vector3 normDir = dir;
	normDir.Normalize();
	float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
	float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
	return Vector3(pitch, yaw, 0.0f); // roll はここでは未使用
}