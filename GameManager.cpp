#include "GameManager.h"

GameManager::GameManager()
{
	///　enum class TEXTUREを参照

	// ブロック
	Game::LoadOBJ("resources/block", "map.obj");
	Game::LoadTexture("resources/block/map.png");
	// 駒
	Game::LoadOBJ("resources/Charactor/king", "king.obj");
	Game::LoadTexture("resources/Charactor/king/king.png");
	Game::LoadOBJ("resources/Charactor/Queen", "Queen.obj");
	Game::LoadTexture("resources/Charactor/Queen/Queen.png");
	Game::LoadOBJ("resources/Charactor/Bishop", "Bishop.obj");
	Game::LoadTexture("resources/Charactor/Bishop/Bishop.png");
	Game::LoadOBJ("resources/Charactor/Knight", "Knight.obj");
	Game::LoadTexture("resources/Charactor/Knight/Knight.png");
	Game::LoadOBJ("resources/Charactor/Rook", "Rook.obj");
	Game::LoadTexture("resources/Charactor/Rook/Rook.png");
	Game::LoadOBJ("resources/Charactor/Pawn", "Pawn.obj");
	Game::LoadTexture("resources/Charactor/Pawn/Pawn.png");

	// フェーズ選択用のアイコン？
	Game::LoadOBJ("resources/PhaseIcon/Buttle", "Buttle.obj");
	Game::LoadTexture("resources/PhaseIcon/Buttle/Buttle.png");
	Game::LoadOBJ("resources/PhaseIcon/Buttle", "Buttle.obj");
	Game::LoadTexture("resources/PhaseIcon/Buttle/Buttle.png");
	Game::LoadOBJ("resources/PhaseIcon/Buttle", "Buttle.obj");
	Game::LoadTexture("resources/PhaseIcon/Buttle/Buttle.png");



	// CharacterManager
	characterManager_ = new CharacterManager();

	// フェーズ管理用
	requestPhase_ = PHASE::Phase_UnitOverview;
	title_ = new Title();
	actSelect_ = new ActSelect();
	stageSelect_ = new StageSelect();
	gameScene_ = new GameScene(characterManager_);
	unitOverview_ = new UnitOverview(characterManager_);
	gatya_ = new Gatya(characterManager_);







}

GameManager::~GameManager()
{
	delete characterManager_;
	characterManager_ = nullptr;

	delete title_;
	title_ = nullptr;
	delete actSelect_;
	actSelect_ = nullptr;
	delete stageSelect_;
	stageSelect_ = nullptr;
	delete gameScene_;
	gameScene_ = nullptr;
	delete unitOverview_;
	unitOverview_ = nullptr;
	delete gatya_;
	gatya_ = nullptr;
}

void GameManager::Update()
{
	if (requestPhase_ != PHASE::Phase_None)
	{
		switch (requestPhase_)
		{
		case PHASE::Phase_None:
			break;
		case PHASE::Phase_Title:
			title_->Initialize();
			break;
		case PHASE::Phase_ActSelect:
			actSelect_->Initialize();
			break;
		case PHASE::Phase_StageSelect:
			stageSelect_->Initialize();
			break;
		case PHASE::Phase_GameScene:
			gameScene_->Initialize();
			break;
		case PHASE::Phase_UnitOverview:
			unitOverview_->Initialize();
			break;
		case PHASE::Phase_Gatya:
			gatya_->Initialize();
			break;
		default:
			break;
		}

		phase_ = requestPhase_;
		requestPhase_ = PHASE::Phase_None;
	}

	switch (phase_)
	{
	case PHASE::Phase_None:
		break;
	case PHASE::Phase_Title:
		title_->Update();
		if (title_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = title_->ChangePhase();
		}
		break;
	case PHASE::Phase_ActSelect:
		actSelect_->Update();
		if (actSelect_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = actSelect_->ChangePhase();
		}
		break;
	case PHASE::Phase_StageSelect:
		stageSelect_->Update();
		if (stageSelect_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = stageSelect_->ChangePhase();
		}
		break;
	case PHASE::Phase_GameScene:
		gameScene_->Update();
		if (gameScene_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = gameScene_->ChangePhase();
		}
		break;
	case PHASE::Phase_UnitOverview:
		unitOverview_->Update();
		if (unitOverview_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = unitOverview_->ChangePhase();
		}
		break;
	case PHASE::Phase_Gatya:
		gatya_->Update();
		if (gatya_->ChangePhase() != PHASE::Phase_None)
		{
			requestPhase_ = gatya_->ChangePhase();
		}
		break;
	default:
		break;
	}

	ImGui::Begin("FPS");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void GameManager::Draw()
{
	switch (phase_)
	{
	case PHASE::Phase_None:
		break;
	case PHASE::Phase_Title:
		title_->Draw();
		break;
	case PHASE::Phase_ActSelect:
		actSelect_->Draw();
		break;
	case PHASE::Phase_StageSelect:
		stageSelect_->Draw();
		break;
	case PHASE::Phase_GameScene:
		gameScene_->Draw();
		break;
	case PHASE::Phase_UnitOverview:
		unitOverview_->Draw();
		break;
	case PHASE::Phase_Gatya:
		gatya_->Draw();
		break;
	default:
		break;
	}
}