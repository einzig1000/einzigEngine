#include "GameManager.h"

GameManager::GameManager()
{
	skyDome_.model = ResourceID::GetModelID(ModelID::Cube);
	skyDome_.texture = ResourceID::GetTextureID(BlockID::Stone);
	skyDome_.additionalTexture = ResourceID::GetTextureID(TextureID::monsterBall);
	skyDome_.scale.value = Vector3(100.0f, 100.0f, 100.0f);
	skyDome_.name = "SkyDome";

	// フェーズクラス生成
	testPhase_ = std::make_unique<TestPhase>();
	battlePhase_ = std::make_unique<BattlePhase>();
	titlePhase_ = std::make_unique<TitlePhase>();
	gameScenePhase_ = std::make_unique<GameScenePhase>();

	// 初期フェーズ設定
	requestPhase_ = PHASE::Phase_Test;
}

GameManager::~GameManager()
{
}

void GameManager::Update()
{
	// 現在フェーズの更新
	switch (phase_)
	{
	case PHASE::Phase_None:
	{
		break;
	}
	case PHASE::Phase_Test:
	{
		testPhase_->Update();
		if (testPhase_->GetNextPhase() != PHASE::Phase_None)
		{
			requestPhase_ = testPhase_->GetNextPhase();
		}
		break;
	}
	case PHASE::Phase_Title:
	{
		titlePhase_->Update();
		if (titlePhase_->GetNextPhase() != PHASE::Phase_None)
		{
			requestPhase_ = titlePhase_->GetNextPhase();
		}
		break;
	}
	case PHASE::Phase_GameScene:
	{
		gameScenePhase_->Update();
		if (gameScenePhase_->GetNextPhase() != PHASE::Phase_None)
		{
			requestPhase_ = gameScenePhase_->GetNextPhase();
		}
		break;
	}
	default:
	{
		break;
	}
	}

	// フェーズ切り替え要求があった場合
	if (requestPhase_ != PHASE::Phase_None)
	{
		switch (requestPhase_)
		{
		case PHASE::Phase_None:
		{
			break;
		}
		case PHASE::Phase_Test:
		{
			testPhase_->Initialize();
			break;
		}
		case PHASE::Phase_Title:
		{
			titlePhase_->Initialize();
			break;
		}
		case PHASE::Phase_GameScene:
		{
			gameScenePhase_->Initialize();
			break;
		}
		default:
		{
			break;
		}
		}
		// フェーズ更新
		phase_ = requestPhase_;
		// リクエストフェーズ初期化
		requestPhase_ = PHASE::Phase_None;
	}
}

void GameManager::Draw()
{
	// 現在フェーズの描画
	switch (phase_)
	{
	case PHASE::Phase_None:
		break;
	case PHASE::Phase_Test:
		testPhase_->Draw();
		break;
	case PHASE::Phase_Title:
		titlePhase_->Draw();
		break;
	case PHASE::Phase_GameScene:
		gameScenePhase_->Draw();
		break;
	}

	//skyDome_.Draw();
	//skyDome_.DrawImGui();

}