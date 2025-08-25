#include "GameScene.h"
#include "CharacterManager.h"
#include "Camera/CameraController.h"

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

	finishSetUp = false;

	ActSelectIcon_[0].model = uint32_t(TEXTURE::ActSelect_Icon_None);
	ActSelectIcon_[1].model = uint32_t(TEXTURE::ActSelect_Icon_Attack);
	ActSelectIcon_[2].model = uint32_t(TEXTURE::ActSelect_Icon_Move);
	ActSelectIcon_[3].model = uint32_t(TEXTURE::ActSelect_Icon_ChangeCameraMode);

	ActSelectIcon_[0].texture = uint32_t(TEXTURE::ActSelect_Icon_None);
	ActSelectIcon_[1].texture = uint32_t(TEXTURE::ActSelect_Icon_Attack);
	ActSelectIcon_[2].texture = uint32_t(TEXTURE::ActSelect_Icon_Move);
	ActSelectIcon_[3].texture = uint32_t(TEXTURE::ActSelect_Icon_ChangeCameraMode);

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

	finishSetUp = false;

	// アクションディレイ順に行動するために初期化
	for (int i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->states_buttle_ = characterManager_->GetAllCharactor()[i]->states_default_;
		characterManager_->GetAllCharactor()[i]->actionDelay = 100 - characterManager_->GetAllCharactor()[i]->states_default_.speed;
	}
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
		case GameScenePhase::Check:
			Initialize_Check();
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
		Update_Setup();
		break;
	case GameScenePhase::Check:
		Update_Check();
		break;
	case GameScenePhase::PlayerTurn:
		Update_PlayerTurn();
		break;
	case GameScenePhase::EnemyTurn:
		Update_EnemyTurn();
		break;
	case GameScenePhase::Result:
		Update_Result();
		break;
	default:
		break;
	}

	// カメライージング更新用フレーム
	frame_camera++;

    map_->Update();
}

void GameScene::Draw()
{
	// マップの描画
    map_->Draw();
	// アクションディレイ順番の描画


	switch (phase_)
	{
	case GameScenePhase::None:
		break;
	case GameScenePhase::Setup:
		Draw_Setup();
		break;
	case GameScenePhase::Check:
		Draw_Check();
		break;
	case GameScenePhase::PlayerTurn:
		Draw_PlayerTurn();
		break;
	case GameScenePhase::EnemyTurn:
		Draw_EnemyTurn();
		break;
	case GameScenePhase::Result:
		Draw_Result();
		break;
	default:
		break;
	}


	for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	{
		std::string label = "test:" + std::to_string(i);
		ImGui::DragFloat(
			label.c_str(),
			&characterManager_->GetButtleCharactor()[i]->actionDelay
		);
	}
}


void GameScene::Initialize_Check()
{}

void GameScene::Update_Check()
{
	// アクションディレイソート
	characterManager_->Sort_Buttle_ActionDelay();
	// フェーズ更新
	if (characterManager_->GetButtleCharactor()[0]->EnemyOrPlayer_)
	{
		// 次に行動するキャラがプレイヤー
		targetphase_ = GameScenePhase::PlayerTurn;
	}
	else
	{
		targetphase_ = GameScenePhase::EnemyTurn;
	}
}

void GameScene::Draw_Check()
{}


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

void GameScene::Update_Setup()
{
	if (!finishSetUp)
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
							characterManager_->GetAllCharactor()[i]->data.transforms.translate = PositionByIndex(Vector2int{ x,y }, map_->BlockTypeByIndex(Vector2int{ x,y }));
							characterManager_->AddButtleCharactorList(characterManager_->GetAllCharactor()[i]);
							isSet[i] = true;
							y = MAP_HEIGHT;
							x = MAP_WIDTH;
							// 場のキャラが増える度にアクションディレイ順にソート
							characterManager_->Sort_Buttle_ActionDelay();
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
			finishSetUp = true;
			frame_camera = 0;
			frameMAX_camera = 180;
			Game::MoveCenterTarget({ -11.4f, -0.0f, -4.10f }, frameMAX_camera, EaseType::OUT_QUART);
			Game::MoveRotateTarget({ 1.1f, std::numbers::pi * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
			Game::MoveDistanceTarget(32.40f, frameMAX_camera, EaseType::OUT_QUART);
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
				}
			}
		}
	}
	else
	{
		for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
		{
			if (i * 3 < frame_camera)
			{
				characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.x = 
					Easings::EasingFloat(145, -138, EaseType::OUT_CUBIC, float(frame_camera - (i * 3)) / float(frameMAX_camera - 140));
				characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = 
					Easings::EasingFloat(70, -213, EaseType::OUT_CUBIC, float(frame_camera - (i * 3)) / float(frameMAX_camera - 140));
			}
		}
		if (frame_camera > frameMAX_camera)
		{
			targetphase_ = GameScenePhase::PlayerTurn;

			// キャラクターデフォルトステータスをバトル内ステータスに適用
			characterManager_->SetStatus();
			// アクションディレイ順に行動するために初期化
			characterManager_->Sort_Buttle_ActionDelay();
			// カメラモードを俯瞰モードに
			cameraMode = CameraMode_Over_or_Focus::Over;
		}
	}
}

void GameScene::Draw_Setup()
{
	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		// 全てのキャラの2Dスプライトを描画
		characterManager_->GetAllCharactor()[i]->dataSeat.Draw();
		// マップ上に配置されたら3Dモデルを描画
		if (isSet[i])characterManager_->GetAllCharactor()[i]->data.Draw();
		// 配置されてなかったらアイコン描画
		else characterManager_->GetAllCharactor()[i]->dataSeat_type.Draw();
	}
}


void GameScene::Initialize_PlayerTurn()
{
	frame_camera = 60;
	frameMAX_camera = 60;
	Act = CharactorSelectPattern::None;
	moveTragetCoolTome = 0;
	ChangeFocus();
}

void GameScene::Update_PlayerTurn()
{
	Update_FocusMode();

	// 移動
	if (Act == CharactorSelectPattern::Move)
	{
		if (moveTragetCoolTome > 5)
		{
			if (R && moveTargetPositionIndex.x > 0)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x - 1, moveTargetPositionIndex.y };
				// 移動できるか
				if (map_->A_to_B(from, to))
				{
					// 移動範囲をこえてないか
					if (map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), to) <= characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint)
					{
						moveTargetPositionIndex.x -= 1;
						moveTragetCoolTome = 0;
						characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
					}
				}
			}
			if (L && moveTargetPositionIndex.x < MAP_WIDTH - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x + 1, moveTargetPositionIndex.y };
				if (map_->A_to_B(from, to))
				{
					if (map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), to) <= characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint)
					{
						moveTargetPositionIndex.x += 1;
						moveTragetCoolTome = 0;
						characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
					}
				}
			}
			if (T && moveTargetPositionIndex.y > 0)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y - 1 };
				if (map_->A_to_B(from, to))
				{
					if (map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), to) <= characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint)
					{

						moveTargetPositionIndex.y -= 1;
						moveTragetCoolTome = 0;
						characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
					}
				}
			}
			if (B && moveTargetPositionIndex.y < MAP_HEIGHT - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y + 1 };
				if (map_->A_to_B(from, to))
				{
					if (map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), to) <= characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint)
					{
						moveTargetPositionIndex.y += 1;
						moveTragetCoolTome = 0;
						characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
					}
				}
			}
		}
		if (GetHitKey::keys[DIK_SPACE])
		{
			// 移動してたら
			if (IndexByPosition(characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate) != IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate))
			{
				// 何マス移動したかの計算
				//int totalMove = map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), IndexByPosition(characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate));
				int moveCost = 20;
				// 実際に移動
				characterManager_->GetButtleCharactor()[0]->data.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				// 素早さ補正の計算
				float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
				// アクションディレイの更新
				characterManager_->GetButtleCharactor()[0]->actionDelay += moveCost * percentage;
				// フェーズ更新
				targetphase_ = GameScenePhase::Check;
				Act = CharactorSelectPattern::None;
			}
			// 移動先が変わってなかったら
			else
			{
				ChangeFocus();
			}
		}
		moveTragetCoolTome++;
	}
}

void GameScene::Draw_PlayerTurn()
{
	if (Act == CharactorSelectPattern::Move)
	{
		characterManager_->GetButtleCharactor()[0]->targetdata.Draw();
	}

	for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	{
		characterManager_->GetButtleCharactor()[i]->data.Draw();
	}
}


void GameScene::Initialize_EnemyTurn()
{
	frame_camera = 60;
	frameMAX_camera = 60;
}

void GameScene::Update_EnemyTurn()
{
	Update_FocusMode();

	

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

void GameScene::Draw_EnemyTurn()
{
	for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	{
		characterManager_->GetButtleCharactor()[i]->data.Draw();
	}
}

void GameScene::Initialize_Result()
{}

void GameScene::Update_Result()
{


	// フェーズ更新
	if (GetHitKey::keys[DIK_P] && !GetHitKey::preKeys[DIK_P])
	{
		nextPhase = PHASE::Phase_StageSelect;
	}
}

void GameScene::Draw_Result()
{}


void GameScene::Update_FocusMode()
{
	// 俯瞰中
	if (cameraMode == CameraMode_Over_or_Focus::Over)
	{
		for (int i = 0; i < 4; ++i)
		{
			ActSelectIcon_[i].Draw();

			ActSelectIcon_[i].transforms.scale = Easings::EasingVector3({ 1.5f,1.5f,1.5f }, { 0.0f,0.0f,0.0f }, EaseType::LINEAR, float(frame_camera) / float(frameMAX_camera - 20));
			ActSelectIcon_[i].transforms.translate = Easings::EasingVector3(ActSelectIcon_targetIcon[i], characterManager_->GetButtleCharactor()[0]->data.transforms.translate, EaseType::LINEAR, float(frame_camera) / float(frameMAX_camera - 20));
		}
	}
	// １キャラ注視中
	else if (cameraMode == CameraMode_Over_or_Focus::Focus)
	{
		for (int i = 0; i < 4; ++i)
		{
			ActSelectIcon_[i].Draw();

			ActSelectIcon_[i].transforms.scale = Easings::EasingVector3({ 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f }, EaseType::LINEAR, float(frame_camera) / float(frameMAX_camera - 20));
			ActSelectIcon_[i].transforms.translate = Easings::EasingVector3(characterManager_->GetButtleCharactor()[0]->data.transforms.translate , ActSelectIcon_targetIcon[i], EaseType::LINEAR, float(frame_camera) / float(frameMAX_camera - 20));
			ActSelectIcon_[i].LookAtOnce(Game::GetCamera()->transform_.translate);

			if (Game::IsCollisionMouseRayAABB(ActSelectIcon_[i].model, ActSelectIcon_[i].transforms))
			{
				ActSelectIcon_[i].color = 0xFFFFFFFF;
				if (Game::GetMousePress(0))
				{
					// 視点切り替え
					if (i == 0)
					{
						ChangeOver();
					}
					// 移動
					else if (i == 1)
					{
						ChangeOver();
						Act = CharactorSelectPattern::Move;
						moveTargetPosition = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
						moveTargetPositionIndex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
						characterManager_->GetButtleCharactor()[0]->targetdata = characterManager_->GetButtleCharactor()[0]->data;
						characterManager_->GetButtleCharactor()[0]->targetdata.color = 0xFFFFFF55;
					}
				}
			}
			else
			{
				ActSelectIcon_[i].color = 0xFFFFFF30;
			}
		}
	}
}

void GameScene::ChangeOver()
{
	frame_camera = 0;
	frameMAX_camera = 40;
	cameraMode = CameraMode_Over_or_Focus::Over;
	Game::MoveCenterTarget({ -11.4f, -0.0f, -4.10f }, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 1.1f, std::numbers::pi * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(32.40f, frameMAX_camera, EaseType::OUT_QUART);
}

void GameScene::ChangeFocus()
{
	frame_camera = 0;
	frameMAX_camera = 40;
	cameraMode = CameraMode_Over_or_Focus::Focus;
	Game::MoveCenterTarget(characterManager_->GetButtleCharactor()[0]->data.transforms.translate, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 0.8f, std::numbers::pi * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(15.60f, frameMAX_camera, EaseType::OUT_QUART);
	// アイコン座標
	for (int i = 0; i < 4; ++i)
	{
		ActSelectIcon_[i].transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		ActSelectIcon_[i].transforms.scale = { 0.0f,0.0f,0.0f };
		ActSelectIcon_targetIcon[i] = ActSelectIcon_[i].transforms.translate;
	}
	ActSelectIcon_targetIcon[0].x -= 4.0f;
	ActSelectIcon_targetIcon[0].y += 2.0f;

	ActSelectIcon_targetIcon[1].x += 4.0f;
	ActSelectIcon_targetIcon[1].y += 2.0f;

	ActSelectIcon_targetIcon[2].x -= 1.5f;
	ActSelectIcon_targetIcon[2].y += 3.0f;

	ActSelectIcon_targetIcon[3].x += 1.5f;
	ActSelectIcon_targetIcon[3].y += 3.0f;
}







