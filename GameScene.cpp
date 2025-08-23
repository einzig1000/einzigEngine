#include "GameScene.h"
#include "CharacterManager.h"

GameScene::GameScene(CharacterManager* characterManager)
    : characterManager_(characterManager)
{
    nextPhase = PHASE::Phase_None;

    map_ = new Map();

	frameMAX_camera = 0;
	frame_camera = 0;
	centerIndex_camera = { MAP_WIDTH - 1,MAP_HEIGHT - 1 };

	phase_ = GameScenePhase::None;
	targetphase_ = GameScenePhase::Setup;

	holdNumber = -10;

	for (int i = 0; i < 100; ++i)
	{
		isSet[i] = false;
	}

	R = false;
	L = false;
	T = false;
	B = false;
}

GameScene::~GameScene()
{
    delete map_;
    map_ = nullptr;
}

void GameScene::Initialize(int stageNum)
{
    nextPhase = PHASE::Phase_None;

	Initialize_Setup();

    map_->LoadMap(stageNum);

}

void GameScene::Update()
{
	// キー更新をまっさきに
	R = false;
	if (GetHitKey::keys[DIK_D] || GetHitKey::keys[DIK_RIGHTARROW])
	{
		R = true;
	}

	L = false;
	if (GetHitKey::keys[DIK_A] || GetHitKey::keys[DIK_LEFTARROW])
	{
		L = true;
	}

	T = false;
	if (GetHitKey::keys[DIK_W] || GetHitKey::keys[DIK_UPARROW])
	{
		T = true;
	}

	B = false;
	if (GetHitKey::keys[DIK_S] || GetHitKey::keys[DIK_DOWNARROW])
	{
		B = true;
	}



	if (targetphase_ != GameScenePhase::None)
	{
		switch (targetphase_)
		{
		case GameScenePhase::None:
			break;
		case GameScenePhase::Setup:
			Initialize_Setup();
			break;
		case GameScenePhase::PlayerTurn:
			Initialize_PlayerTurn();
			break;
		case GameScenePhase::EnemyTurn:
			Initialize_EnemyTurn();
			break;
		case GameScenePhase::Result:
			Initialize_Result();
			break;
		default:
			break;
		}

		phase_ = targetphase_;
		targetphase_ = GameScenePhase::None;
	}

	switch (phase_)
	{
	case GameScenePhase::None:
		break;
	case GameScenePhase::Setup:
		Updata_Setup();
		break;
	case GameScenePhase::PlayerTurn:
		Updata_PlayerTurn();
		break;
	case GameScenePhase::EnemyTurn:
		Updata_EnemyTurn();
		break;
	case GameScenePhase::Result:
		Updata_Result();
		break;
	default:
		break;
	}

	// カメライージング更新用レーム
	frame_camera++;

    map_->Update();
}

void GameScene::Draw()
{
    map_->Draw();

    // キャラの描画
	if (phase_ == GameScenePhase::PlayerTurn || phase_ == GameScenePhase::EnemyTurn)
	{
		for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
		{
			characterManager_->GetButtleCharactor()[i]->data.Draw();
		}
	}
	else if (phase_ == GameScenePhase::Setup)
	{
		for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
		{
			if (isSet[i])characterManager_->GetAllCharactor()[i]->data.Draw();
			characterManager_->GetAllCharactor()[i]->dataSeat.Draw();
			if (!isSet[i])characterManager_->GetAllCharactor()[i]->dataSeat_type.Draw();
		}
	}
}

void GameScene::Initialize_Setup()
{
	Game::SetControlModeCameraCenter(false);
	Game::SetControlModeCameraRotate(false);
	Game::SetControlModeCameraDistance(true);

	frame_camera = 0;
	frameMAX_camera = 60;
	Game::MoveCenterTarget(PositionByIndex(centerIndex_camera), frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(15.60f, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 1.13f, 0.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);


	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.x = 145;
		characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.y = i * 40 + 30;
		characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = 70;
		characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.y = i * 40 + 30;
	}
}

void GameScene::Updata_Setup()
{
	// カメラコントロール
	if (frame_camera > frameMAX_camera)
	{
		if (R || L || T || B)
		{
			bool easingsSet = false;

			if (R && centerIndex_camera.x > 0)
			{
				centerIndex_camera.x -= 1;
				easingsSet = true;
			}
			if (L && centerIndex_camera.x < MAP_WIDTH - 1)
			{
				centerIndex_camera.x += 1;
				easingsSet = true;
			}
			if (T && centerIndex_camera.y > 0)
			{
				centerIndex_camera.y -= 1;
				easingsSet = true;
			}
			if (B && centerIndex_camera.y < MAP_HEIGHT - 1)
			{
				centerIndex_camera.y += 1;
				easingsSet = true;
			}

			if (easingsSet)
			{
				frame_camera = 0;
				frameMAX_camera = 5;
				Game::MoveCenterTarget(PositionByIndex(centerIndex_camera), frameMAX_camera, EaseType::LINEAR);
			}
		}
	}

	// キャラ詳細シートの更新(シートとマウスが衝突している間そのシートは0xFF0000FFになる。その状態でクリックするとクリックしている間のみアイコン座標を操作できるようになる。アイコンを離した位置にあるマップに3D駒を配置する)
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		// マウスと衝突してるスプライト
		if (characterManager_->GetAllCharactor()[i]->dataSeat.colisionMouseRay)
		{
			characterManager_->GetAllCharactor()[i]->dataSeat.color = 0xFF0000FF;

			// 衝突中にクリック
			if (Game::GetMousePress(0) && holdNumber < 0)
			{
				holdNumber = i;
			}
		}
		// 衝突していないスプライト
		else
		{
			characterManager_->GetAllCharactor()[i]->dataSeat.color = 0xFFFFFFFF;
		}

		// 掴んでる間
		if (i == holdNumber)
		{
			characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = Game::GetMousePosition().x;
			characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.y = Game::GetMousePosition().y;
		}
		// 離した瞬間
		if (i == holdNumber && !Game::GetMousePress(0))
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					if (map_->data[y][x].color == 0xFF0000FF)
					{
						int Hight = 0;
						if (map_->BlockTypeByIndex(Vector2int{ x,y }) == BLOCK_TYPE::Wall)Hight = 1;
						characterManager_->GetAllCharactor()[i]->data.transforms.translate = PositionByIndex(Vector2int{ x,y }, Hight);
						characterManager_->AddButtleCharactorList(characterManager_->GetAllCharactor()[i]);
						isSet[i] = true;
						y = MAP_HEIGHT;
						x = MAP_WIDTH;
					}
					// 設置場所が見つからなかった
					if (y == MAP_HEIGHT - 1 && x == MAP_WIDTH - 1 && isSet[i] == false)
					{
						characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = 70;
						characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.y = i * 40 + 30;
					}
				}
			}
		}
	}

	//-------- x=0y=0から調査しているせいで最初にマウスレイと衝突した一番手前のブロックじゃなくて --------
	// マップとマウスの当たり判定
	bool TheOne = false;
	for (uint32_t y = 0; y < MAP_HEIGHT; ++y)
	{
		for (uint32_t x = 0; x < MAP_WIDTH; ++x)
		{
			// 色の初期化
			map_->data[y][x].color = 0xFFFFFFFF;
			// 駒をおける場所か判断
			if (map_->BlockEffectByIndex(Vector2int(x, y)) == BLOCK_EFFECT_TYPE::AbleCharactorSet)
			{
				// まだ色を変えられたマスがない（TheOneがfalse）なら色を変える
				if (!TheOne && IsCollision(Game::GetMouseRay(), map_->data[y][x].AABB))
				{
					// おける場所だったので色を変える
					map_->data[y][x].color = 0xFF0000FF;
					//TheOne = true;
				}
			}
		}
	}

	// クリックされていない時holdNumberは-10に初期化される
	if (holdNumber >= 0)
	{
		if (!Game::GetMousePress(0))
		{
			holdNumber = -10;
		}
	}
	
	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::PlayerTurn;
	}
}

void GameScene::Initialize_PlayerTurn()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
		}
	}

	frame_camera = 0;
	frameMAX_camera = 60;
	Game::MoveCenterTarget({ 0.0f, -0.0f, -5.390f }, 200, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 0.4f, -std::numbers::pi / 2.0f, 0.0f }, 200, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(15.60f, 200, EaseType::OUT_QUART);
}

void GameScene::Updata_PlayerTurn()
{



	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::EnemyTurn;
	}
	else if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::Result;
	}
}

void GameScene::Initialize_EnemyTurn()
{}

void GameScene::Initialize_Result()
{}



void GameScene::Updata_EnemyTurn()
{



	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::PlayerTurn;
	}
	else if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::Result;
	}
}

void GameScene::Updata_Result()
{


	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		nextPhase = PHASE::Phase_StageSelect;
	}
}

		//std::string label = "test" + std::to_string(i);
		//
		//// ImGui に渡すラベルは const char*
		//ImGui::DragFloat3(
		//	label.c_str(),
		//	&characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.x
		//);