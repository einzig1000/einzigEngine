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
			characterManager_->GetAllCharactor()[i]->data.Draw();
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
	Game::MoveCenterTarget(PositionByIndex(centerIndex_camera), frameMAX_camera);
	Game::MoveDistanceTarget(15.60f, frameMAX_camera);
	Game::MoveRotateTarget({ 1.13f, 0.0f, 0.0f }, frameMAX_camera);
}

void GameScene::Initialize_PlayerTurn()
{}

void GameScene::Initialize_EnemyTurn()
{}

void GameScene::Initialize_Result()
{}

void GameScene::Updata_Setup()
{
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
				Game::MoveCenterTarget(PositionByIndex(centerIndex_camera), frameMAX_camera);
			}
		}


	}


	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		targetphase_ = GameScenePhase::PlayerTurn;
	}
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
