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

	// カメライージング
	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200);
	Game::MoveRotateTarget({ 0.4f, -std::numbers::pi / 2.0f, 0.0f }, 200);
	Game::MoveDistanceTarget(15.60f, 200);
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
	// イージングの設定
	tMAX = 10;
	t = tMAX;

	// 注目列の初期化
	targetY = 0;

	// 回転中心イージング前と後の更新
	centerPreTransforms = center.transforms;
	centerTargetTransforms = centerPreTransforms;
	centerTargetTransforms.rotate.y = 10.0f;

	// マップ座標イージング前と後の更新
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			mapPreTransforms[y][x] = map_->data[y][x].transforms;
			mapTargetTransforms[y][x] = mapPreTransforms[y][x];
			map_->data[y][x].transforms.parentWorld = nullptr;
		}
	}

	// キャラが乗ってるマップのみ更新
	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
				map_->data[y][x].LookAtFront();
			if (check < CharSum)
			{
			}
			check++;
		}
	}
}

void UnitOverview::Updata_ALL()
{
	// 入力受付
	int deltaWheel = Game::GetMouseWheel();
	bool up = false;
	if (GetHitKey::keys[DIK_UPARROW] || GetHitKey::keys[DIK_W] || deltaWheel > 0)up = true;
	bool down = false;
	if (GetHitKey::keys[DIK_DOWNARROW] || GetHitKey::keys[DIK_S] || deltaWheel < 0)down = true;

	// マップイージング前と後の更新 + イージング係数の初期化
	if (t > tMAX)
	{
		if (up || down)
		{
			if (up)targetY++;
			else targetY--;

			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					mapPreTransforms[y][x] = map_->data[y][x].transforms;
					mapTargetTransforms[y][x] = mapPreTransforms[y][x];

					if (up)
					{
						mapTargetTransforms[y][x].translate.x += BLOCK_HEIGHT;
						if (x < targetY)
						{
							mapTargetTransforms[y][x].translate.x += BLOCK_HEIGHT * 5;
						}
					}
					else
					{
						mapTargetTransforms[y][x].translate.x -= BLOCK_HEIGHT;
						if (x < targetY + 1)
						{
							mapTargetTransforms[y][x].translate.x -= BLOCK_HEIGHT * 5;
						}
					}
				}
			}
			t = 0;
		}
	}

	// 計数1.0f以下の時のみイージング
	if (t < tMAX)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				map_->data[y][x].transforms.translate.x = Easings::EasingFloat(mapPreTransforms[y][x].translate.x, mapTargetTransforms[y][x].translate.x, EaseType::LINEAR, float(t) / tMAX);
				map_->data[y][x].LookAtFront();
			}
		}
	}

	// イージング係数更新
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
	// イージングの設定
	tMAX = 100;
	t = 0;

	// 回転中心イージング前と後の更新
	centerPreTransforms = center.transforms;
	centerTargetTransforms = centerPreTransforms;
	centerTargetTransforms.translate.y = 10.0f;

	// マップ座標イージング前と後の更新
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			mapPreTransforms[y][x] = map_->data[y][x].transforms;
			mapTargetTransforms[y][x] = mapPreTransforms[y][x];
		}
	}

	// キャラが乗ってるマップのみ更新
	int check = 0;
	int CharSum = characterManager_->GetAllCharactor().size();
	Vector3 IconSum[100];
	deltaRotate = (2.0f * std::numbers::pi) / CharSum;
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (check < CharSum)
			{
				// 回転中心を親に
				map_->data[y][x].transforms.parentWorld = &center.transforms.World;
				// イージング先の設定
				float angle = deltaRotate * check;
				IconSum[check].x = -std::sinf(angle);
				IconSum[check].z = -std::cosf(angle);
				IconSum[check] *= 10;
				mapTargetTransforms[y][x].translate = IconSum[check];
			}
			check++;
		}
	}

	// カメライージング
	Game::MoveRotateTarget({ 0,float(-std::numbers::pi) + (deltaRotate * float(targetChar)),0 }, tMAX);
	Game::MoveCenterTarget({ 0,11,0 }, tMAX + 60);
	Game::MoveDistanceTarget( -float(CharSum) / 10.0f, tMAX);
}

void UnitOverview::Updata_ALL_UNIT()
{
	// 計数1.0f以下の時のみイージング
	if (t < tMAX)
	{
		// マップの操作
		int check = 0;
		int CharSum = characterManager_->GetAllCharactor().size();
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				map_->data[y][x].transforms.translate.x = Easings::EasingFloat(mapPreTransforms[y][x].translate.x, mapTargetTransforms[y][x].translate.x, EaseType::LINEAR, float(t) / tMAX);
				map_->data[y][x].transforms.translate.y = Easings::EasingFloat(mapPreTransforms[y][x].translate.y, mapTargetTransforms[y][x].translate.y, EaseType::LINEAR, float(t) / tMAX);
				map_->data[y][x].transforms.translate.z = Easings::EasingFloat(mapPreTransforms[y][x].translate.z, mapTargetTransforms[y][x].translate.z, EaseType::LINEAR, float(t) / tMAX);
				if (check < CharSum)map_->data[y][x].LookAtOnce(center.transforms.translate);
				check++;
			}
		}

		// センターの操作
		center.transforms.translate.y = Easings::EasingFloat(centerPreTransforms.translate.y, centerTargetTransforms.translate.y, EaseType::LINEAR, float(t) / tMAX);
	}

	// イージング係数更新
	t++;

	// 状態遷移フェーズの修了
	if (float(t) > tMAX)targetphase_ = ViewPhase::UNIT;
}


void UnitOverview::Initialize_UNIT()
{
	// イージングの設定
	tMAX = 10;
	t = tMAX;

	// 回転中心イージング前と後の更新
	centerPreTransforms = center.transforms;
	centerTargetTransforms = centerPreTransforms;
}

void UnitOverview::Updata_UNIT()
{
	// 入力受付
	int deltaWheel = Game::GetMouseWheel();
	bool right = false;
	if (GetHitKey::keys[DIK_RIGHTARROW] || GetHitKey::keys[DIK_D] || deltaWheel > 0)right = true;
	bool left = false;
	if (GetHitKey::keys[DIK_LEFTARROW] || GetHitKey::keys[DIK_A] || deltaWheel < 0)left = true;

	// 回転中心イージング前と後の更新 + イージング係数の初期化
	if (t > tMAX)
	{
		if (right || left)
		{
			centerPreTransforms = center.transforms;
			centerTargetTransforms = centerPreTransforms;
			if (right)centerTargetTransforms.rotate.y -= deltaRotate;
			else centerTargetTransforms.rotate.y += deltaRotate;
			t = 0;
		}
	}

	// 計数1.0f以下の時のみイージング
	if (t < tMAX)
	{
		center.transforms.rotate.y = Easings::EasingFloat(centerPreTransforms.rotate.y, centerTargetTransforms.rotate.y, EaseType::LINEAR, float(t) / tMAX);
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

	// イージング係数更新
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
	// イージングの設定
	tMAX = 100;
	t = 0;

	// 回転中心イージング前と後の更新
	centerPreTransforms = center.transforms;
	centerTargetTransforms = centerPreTransforms;
	centerTargetTransforms.translate.y = 0.0f;
	centerTargetTransforms.rotate.y = 0.0f;

	// マップ座標イージング前と後の更新
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			mapPreTransforms[y][x] = map_->data[y][x].transforms;
			mapTargetTransforms[y][x].translate = PositionByIndex(Vector2int(x,y));
		}
	}

	// カメライージング
	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200);
	Game::MoveRotateTarget({ 0.4f, -std::numbers::pi / 2.0f, 0.0f }, 200);
	Game::MoveDistanceTarget(15.60f, 200);
}

void UnitOverview::Updata_UNIT_ALL()
{
	// 計数1.0f以下の時のみイージング
	if (t < tMAX)
	{
		int check = 0;
		int CharSum = characterManager_->GetAllCharactor().size();
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				map_->data[y][x].transforms.translate.x = Easings::EasingFloat(mapPreTransforms[y][x].translate.x, mapTargetTransforms[y][x].translate.x, EaseType::LINEAR, float(t) / tMAX);
				map_->data[y][x].transforms.translate.y = Easings::EasingFloat(mapPreTransforms[y][x].translate.y, mapTargetTransforms[y][x].translate.y, EaseType::LINEAR, float(t) / tMAX);
				map_->data[y][x].transforms.translate.z = Easings::EasingFloat(mapPreTransforms[y][x].translate.z, mapTargetTransforms[y][x].translate.z, EaseType::LINEAR, float(t) / tMAX);

				if (check < CharSum)map_->data[y][x].LookAtFront();
				check++; 
			}
		}

		// センターの操作
		center.transforms.translate.y = Easings::EasingFloat(centerPreTransforms.translate.y, centerTargetTransforms.translate.y, EaseType::LINEAR, float(t) / tMAX);
		center.transforms.rotate.y = Easings::EasingFloat(centerPreTransforms.rotate.y, centerTargetTransforms.rotate.y, EaseType::LINEAR, float(t) / tMAX);
	}

	// イージング係数更新
	t++;


	if (float(t) > tMAX)targetphase_ = ViewPhase::ALL;
}
