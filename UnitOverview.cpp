#include "UnitOverview.h"
#include "CharacterManager.h"
#include "Map.h"

UnitOverview::UnitOverview(CharacterManager* characterManager)
	: characterManager_(characterManager)
{
	nextPhase = PHASE::Phase_None;

	phase_ = ViewPhase::None;
	targetphase_ = ViewPhase::None;

	map_ = new Map();

	t = 0;
}

UnitOverview::~UnitOverview()
{
	delete map_;
	map_ = nullptr;
}


void UnitOverview::Initialize()
{
	nextPhase = PHASE::Phase_None;

	map_->LoadMap(0);
	targetY = 0;
	targetChar = 0;

	int x = 0;
	int y = 0;
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->data.transforms.parentWorld = &map_->data[x][y].transforms.World;
		characterManager_->GetAllCharactor()[i]->data.transforms.translate.z += BLOCK_HEIGHT * 5;
		x++;
		if (x >= MAP_HEIGHT)
		{
			x = 0;
			y ++;
			if (y >= MAP_WIDTH)
			{

			}
		}
	}

	phase_ = ViewPhase::None;
	targetphase_ = ViewPhase::ALL;
}

void UnitOverview::Update()
{
	if (targetphase_ != ViewPhase::None)
	{
		switch (targetphase_)
		{
		case UnitOverview::ViewPhase::None:
			break;
		case UnitOverview::ViewPhase::ALL:
			Initialize_ALL();
			break;
		case UnitOverview::ViewPhase::ALL_UNIT:
			Initialize_ALL_UNIT();
			break;
		case UnitOverview::ViewPhase::UNIT:
			Initialize_UNIT();
			break;
		case UnitOverview::ViewPhase::UNIT_ALL:
			Initialize_UNIT_ALL();
			break;
		default:
			break;
		}

		phase_ = targetphase_;
		targetphase_ = ViewPhase::None;
	}

	switch (phase_)
	{
	case UnitOverview::ViewPhase::None:
		break;
	case UnitOverview::ViewPhase::ALL:
		Updata_ALL();
		break;
	case UnitOverview::ViewPhase::ALL_UNIT:
		Updata_ALL_UNIT();
		break;
	case UnitOverview::ViewPhase::UNIT:
		Updata_UNIT();
		break;
	case UnitOverview::ViewPhase::UNIT_ALL:
		Updata_UNIT_ALL();
		break;
	default:
		break;
	}

	map_->Update();


	ImGui::Begin("view");
	ImGui::Text("targetChar:%d", targetChar);
	ImGui::DragFloat3("translate", &center.transforms.translate.x,0.01f);
	ImGui::DragFloat3("rotate", &map_->data[0][0].transforms.rotate.x, 0.01f);
	ImGui::End();
	
}

void UnitOverview::Draw()
{
	map_->Draw();

	// キャラの描画
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->data.Draw();
	}
	center.Draw();
}


void UnitOverview::Initialize_ALL()
{
	tMAX = 10;
	t = tMAX;
	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			preTransforms[y][x] = map_->data[y][x].transforms;
			targetTransforms[y][x] = preTransforms[y][x];
			map_->data[y][x].transforms.parentWorld = nullptr;;
			if (check < CharSum)map_->data[y][x].LookAtFront();
			check++;
		}
	}
	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200);
	Game::MoveRotateTarget({ 0.4f, -std::numbers::pi / 2.0f, 0.0f }, 200);
	Game::MoveDistanceTarget(15.60f, 200);
}

void UnitOverview::Updata_ALL()
{
	// マップ移動
	preWheel = nowWheel;
	nowWheel = Game::GetMouseWheel();
	if (nowWheel != 0 && preWheel == 0 && t > tMAX)// && targetY >= 0
	{
		if (nowWheel > 0)targetY++;
		else targetY--;
		t = 0;

		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				preTransforms[y][x] = map_->data[y][x].transforms;
				targetTransforms[y][x] = preTransforms[y][x];

				if (nowWheel > 0)
				{
					targetTransforms[y][x].translate.x += BLOCK_HEIGHT;
					if (x < targetY)
					{
						targetTransforms[y][x].translate.x += BLOCK_HEIGHT * 5;
					}
				}
				else
				{
					targetTransforms[y][x].translate.x -= BLOCK_HEIGHT;
					if (x < targetY + 1)
					{
						targetTransforms[y][x].translate.x -= BLOCK_HEIGHT * 5;
					}
				}
			}
		}
	}

	// 常時イージング
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			map_->data[y][x].transforms.translate.x = Easings::F_LINEAR(preTransforms[y][x].translate.x, targetTransforms[y][x].translate.x, float(t) / tMAX);
			map_->data[y][x].transforms.translate.y = Easings::F_LINEAR(preTransforms[y][x].translate.y, targetTransforms[y][x].translate.y, float(t) / tMAX);
			map_->data[y][x].transforms.translate.z = Easings::F_LINEAR(preTransforms[y][x].translate.z, targetTransforms[y][x].translate.z, float(t) / tMAX);
			map_->data[y][x].LookAtFront();
		}
	}
	t++;

	// 駒とマウスの当たり判定
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->data.color = 0xFFFFFFFF;
	}
	for (uint32_t i = targetY * MAP_HEIGHT; i < (targetY + 1) * MAP_HEIGHT; ++i)
	{
		if (i < characterManager_->GetAllCharactor().size())
		{
			Charactor* dex = characterManager_->GetAllCharactor()[i];
			// もしマウスレイと衝突してたら色を変える
			if (IsCollision(Game::GetMouseRay(), dex->data.AABB))
			{
				dex->data.color = 0xFF0000FF;
				targetChar = i;
				// 衝突したときにクリックされたら[i]を保存し演出に以降する
				if (Game::GetMousePress(0))
				{
					targetphase_ = ViewPhase::ALL_UNIT;
				}
				break;
			}
		}
	}
}


void UnitOverview::Initialize_ALL_UNIT()
{
	tMAX = 100;
	t = 0;

	
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			preTransforms[y][x] = map_->data[y][x].transforms;
			targetTransforms[y][x] = preTransforms[y][x];
		}
	}

	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (check < CharSum)
			{
				map_->data[y][x].transforms.parentWorld = &center.transforms.World;
			}
			check++;
		}
	}


	Vector3 IconSum[100];
	deltaRotate = (2.0f * std::numbers::pi) / CharSum;
	for (int i = 0; i < CharSum; ++i)
	{
		float angle = deltaRotate * i;
		IconSum[i].x = -std::sinf(angle);
		IconSum[i].z = -std::cosf(angle);
		//IconSum[i] *= float(CharSum);
		IconSum[i] *= 10;
		//IconSum[i].y = 10.0f;


		int x = i % MAP_HEIGHT;
		int y = i / MAP_HEIGHT;
		targetTransforms[x][y].translate = IconSum[i];
	}


	// カメラの操作

	//Game::MoveRotateTarget({ 0,-(std::numbers::pi),0 }, tMAX);
	//Game::MoveRotateTarget({ 0,-(std::numbers::pi) * 5,0 }, tMAX);
	Game::MoveRotateTarget({ 0,float(-std::numbers::pi) + (deltaRotate * float(targetChar)),0 }, tMAX);

	//float aa = deltaRotate * float(targetChar);
	//int rot = int(aa) % 360;
	//Game::MoveRotateTarget({ 0,float(-std::numbers::pi) + float(rot),0 }, tMAX);

	Game::MoveCenterTarget({ 0,11,0 }, tMAX + 60);
	//Game::MoveCenterTarget({ 0,0,0 }, tMAX + 60);
	Game::MoveDistanceTarget( -float(CharSum) / 10.0f, tMAX);


	center.transforms.translate = { 0,0,0 };
}

void UnitOverview::Updata_ALL_UNIT()
{
	// 常時イージング
	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			map_->data[y][x].transforms.translate.x = Easings::F_LINEAR(preTransforms[y][x].translate.x, targetTransforms[y][x].translate.x, float(t) / tMAX);
			map_->data[y][x].transforms.translate.y = Easings::F_LINEAR(preTransforms[y][x].translate.y, targetTransforms[y][x].translate.y, float(t) / tMAX);
			map_->data[y][x].transforms.translate.z = Easings::F_LINEAR(preTransforms[y][x].translate.z, targetTransforms[y][x].translate.z, float(t) / tMAX);
			if (check < CharSum)map_->data[y][x].LookAtOnce(center.transforms.translate);
			check++;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// センターの操作
	center.transforms.translate.y = Easings::F_LINEAR(0.0f, 10.0f, float(t) / tMAX);
	
	//center.transforms.rotate.y = Easings::LINER(0, (deltaRotate * (targetChar + ((-2 * targetChar) + 3))) + (deltaRotate / 2), float(t) / tMAX);
	//center.transforms.rotate.y = Easings::LINER(0, ((deltaRotate * targetChar) + (deltaRotate / 2)), float(t) / tMAX);
	
	//center.transforms.rotate.y = Easings::LINER(0, ((deltaRotate * (5 - targetChar)) + (deltaRotate / 2)), float(t) / tMAX);
	
	//center.transforms.rotate.y = Easings::LINER(0, (std::numbers::pi * 2.0f), float(t) / tMAX);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	t++;

	if (float(t) > tMAX)targetphase_ = ViewPhase::UNIT;
}


void UnitOverview::Initialize_UNIT()
{
	tMAX = 10;
	t = tMAX;

	centerPreTransforms = center.transforms;
	centerTargetTransforms = centerPreTransforms;
}

void UnitOverview::Updata_UNIT()
{
	int deltaWheel = Game::GetMouseWheel();
	bool right = false;
	if (GetHitKey::keys[DIK_RIGHTARROW] || GetHitKey::keys[DIK_D] || deltaWheel > 0)right = true;
	bool left = false;
	if (GetHitKey::keys[DIK_LEFTARROW] || GetHitKey::keys[DIK_A] || deltaWheel < 0)left = true;

	if ((right || left) && t > tMAX)
	{
		centerPreTransforms = center.transforms;
		centerTargetTransforms = centerPreTransforms;
		if (right)centerTargetTransforms.rotate.y -= deltaRotate;
		else centerTargetTransforms.rotate.y += deltaRotate;
		t = 0;
	}

	if (t < tMAX)
	{
		int check = 0;
		int CharSum = characterManager_->GetAllCharactor().size();
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				if (check < CharSum)map_->data[y][x].LookAtOnce(center.transforms.translate);
				check++;
			}
		}
	}


	center.transforms.rotate.y = Easings::F_LINEAR(centerPreTransforms.rotate.y, centerTargetTransforms.rotate.y, float(t) / tMAX);
	t++;

	bool escape = false;
	if (GetHitKey::keys[DIK_SPACE] || GetHitKey::keys[DIK_ESCAPE])escape = true;
	if (escape)
	{
		targetphase_ = ViewPhase::UNIT_ALL;
	}
}


void UnitOverview::Initialize_UNIT_ALL()
{
	tMAX = 100;
	t = 0;
	targetY = 0;
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			preTransforms[y][x] = map_->data[y][x].transforms;
			targetTransforms[y][x].translate = PositionByIndex(Vector2int(x,y));
		}
	}
	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200);
	Game::MoveRotateTarget({ 0.4f, -std::numbers::pi / 2.0f, 0.0f }, 200);
	Game::MoveDistanceTarget(15.60f, 200);
}

void UnitOverview::Updata_UNIT_ALL()
{
	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			map_->data[y][x].transforms.translate.x = Easings::F_LINEAR(preTransforms[y][x].translate.x, targetTransforms[y][x].translate.x, float(t) / tMAX);
			map_->data[y][x].transforms.translate.y = Easings::F_LINEAR(preTransforms[y][x].translate.y, targetTransforms[y][x].translate.y, float(t) / tMAX);
			map_->data[y][x].transforms.translate.z = Easings::F_LINEAR(preTransforms[y][x].translate.z, targetTransforms[y][x].translate.z, float(t) / tMAX);
			map_->data[y][x].LookAtFront();
		}
	}
	t++;

	center.transforms.translate.y = Easings::F_LINEAR(10.0f, 0.0f, float(t) / tMAX);

	if (t > tMAX)
	{
		targetphase_ = ViewPhase::ALL;
	}
}
