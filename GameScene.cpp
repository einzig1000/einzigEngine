#include "GameScene.h"
#include "CharacterManager.h"
#include "Camera/CameraController.h"
#include <algorithm>

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
	ActSelectIcon_[1].model = uint32_t(TEXTURE::ActSelect_Icon_Skill);
	ActSelectIcon_[2].model = uint32_t(TEXTURE::ActSelect_Icon_Move);
	ActSelectIcon_[3].model = uint32_t(TEXTURE::ActSelect_Icon_ChangeCameraMode);

	ActSelectIcon_[0].texture = uint32_t(TEXTURE::ActSelect_Icon_None);
	ActSelectIcon_[1].texture = uint32_t(TEXTURE::ActSelect_Icon_Skill);
	ActSelectIcon_[2].texture = uint32_t(TEXTURE::ActSelect_Icon_Move);
	ActSelectIcon_[3].texture = uint32_t(TEXTURE::ActSelect_Icon_ChangeCameraMode);

	ActSelectIcon_[0].options.enableLighting = false;
	ActSelectIcon_[1].options.enableLighting = false;
	ActSelectIcon_[2].options.enableLighting = false;
	ActSelectIcon_[3].options.enableLighting = false;
}

GameScene::~GameScene()
{
    delete map_;
    map_ = nullptr;
}

void GameScene::Initialize(int stageNum)
{
    nextPhase = PHASE::Phase_None;

	targetphase_ = GameScenePhase::Setup;

    map_->LoadMap(stageNum);
	finishSetUp = false;

	if (stageNum == 1)centerIndex_camera = { 17,5 };

	// アクションディレイ順に行動するために初期化
	for (int i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->states_buttle_ = characterManager_->GetAllCharactor()[i]->states_default_;
		characterManager_->GetAllCharactor()[i]->actionDelay = 100 - characterManager_->GetAllCharactor()[i]->states_default_.speed;
	}
	for (int i = 0; i < characterManager_->GetAllEnemy().size(); ++i)
	{
		characterManager_->GetAllEnemy()[i]->states_buttle_ = characterManager_->GetAllEnemy()[i]->states_default_;
		characterManager_->GetAllEnemy()[i]->actionDelay = 100 - characterManager_->GetAllEnemy()[i]->states_default_.speed;
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

	// ここで色とか変わってる
	map_->Update();

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


	//for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	//{
	//	std::string label = "test:" + std::to_string(i);
	//	ImGui::DragFloat(
	//		label.c_str(),
	//		&characterManager_->GetButtleCharactor()[i]->actionDelay
	//	);
	//}


	if (Game::GetCamera()->GetDistance() > 32.4f)
	{
		Game::GetCamera()->SetDistance(32.4f);
	}
}

// １キャラ行動する度に呼び出される
void GameScene::Initialize_Check()
{}

void GameScene::Update_Check()
{
	// アクションディレイソート
	characterManager_->Sort_Buttle_ActionDelay();
	// フェーズ更新
	bool enemy = false;
	bool player = true;
	for (int i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	{
		if (characterManager_->GetButtleCharactor()[i]->EnemyOrPlayer_)
		{
			player = true;
		}
		else
		{
			enemy = true;
		}
	}

	// プレイヤーが一人もいない｜｜プレイヤーもエネミーもひとりもいない
	if ((!player && enemy) || (!player && !enemy))
	{
		targetphase_ = GameScenePhase::Result;
	}
	// プレイヤーしかいない
	else if (player && !enemy)
	{
		targetphase_ = GameScenePhase::Result;
	}
	else if (characterManager_->GetButtleCharactor()[0]->EnemyOrPlayer_)
	{
		// 次に行動するキャラがプレイヤー
		targetphase_ = GameScenePhase::PlayerTurn;
	}
	else if (!characterManager_->GetButtleCharactor()[0]->EnemyOrPlayer_)
	{
		targetphase_ = GameScenePhase::EnemyTurn;
	}
}

void GameScene::Draw_Check()
{}

// 駒を配置するフェーズ　各ゲームで一度しか呼び出されない
void GameScene::Initialize_Setup()
{
	frame_camera = 0;
	frameMAX_camera = 60;
	Game::MoveCenterTarget(PositionByIndex(centerIndex_camera), frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(20.0f, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 1.13f, 0.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);

	for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
	{
		characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.x = 145.0f;
		characterManager_->GetAllCharactor()[i]->dataSeat.transforms.translate.y = i * 40.0f + 30.0f;
		characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = 70.0f;
		characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.y = i * 40.0f + 30.0f;
	}

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			map_->IsCollisionMouseRay_[y][x] = false;
			map_->CharactorType[y][x] = BLOCK_CHAR::Empty;

			if (map_->InitializeEnemy[y][x] > 0)
			{
				Charactor* add;
				add = characterManager_->GetAllEnemy()[map_->InitializeEnemy[y][x] - 1]->Clone();
				// 駒の設置
				add->data.transforms.translate = PositionByIndex(Vector2int{ x,y }, map_->BlockTypeByIndex(Vector2int{ x,y }));
				// 駒をバトルキャラリストに追加
				characterManager_->AddButtleCharactorList(add);
				// バトルキャラが増える度にアクションディレイ順にソート
				characterManager_->Sort_Buttle_ActionDelay();
				// 配置した場所にプレイヤーが配置されたことを記録
				map_->CharactorType[y][x] = BLOCK_CHAR::OnEnemy;
			}
		}
	}
}

void GameScene::Update_Setup()
{
	center_camera = Game::GetCamera()->GetCenter();
	if (!finishSetUp)
	{
		// カメラコントロール
		if (frame_camera > frameMAX_camera)
		{
			if (R || L || T || B)
			{
				bool w = false;
				bool h = false;
				if (R && !L && IndexByPosition(center_camera).x > 0)
				{
					center_camera.x += BLOCK_HEIGHT;
					w = true;
				}
				if (L && !R && IndexByPosition(center_camera).x < MAP_WIDTH - 1)
				{
					center_camera.x -= BLOCK_HEIGHT;
					w = true;
				}

				if (T && !B && IndexByPosition(center_camera).y > 0)
				{
					center_camera.z += BLOCK_HEIGHT;
					h = true;
				}
				if (B && !T && IndexByPosition(center_camera).y < MAP_HEIGHT - 1)
				{
					center_camera.z -= BLOCK_HEIGHT;
					h = true;
				}
				frame_camera = 0;
				if (w && h)frameMAX_camera = 14;
				else frameMAX_camera = 10;
				Game::MoveCenterTarget(center_camera, frameMAX_camera, EaseType::LINEAR);
			}
		}

		// キャラ詳細シートの更新
		for (uint32_t i = 0; i < characterManager_->GetAllCharactor().size(); ++i)
		{
			// マウスと衝突してるスプライト
			if (characterManager_->GetAllCharactor()[i]->dataSeat.colisionMouseRay)
			{
				characterManager_->GetAllCharactor()[i]->dataSeat.color = 0xFF0000FF;

				// 衝突中にクリック
				if (Game::GetMousePress(0) && holdNumber < 0)
				{
					// 何番目の駒をクリックしているのか保存
					holdNumber = i;
					// 既にセットされていた時
					if (isSet[i] == true)
					{
						// リセット
						isSet[i] = false;
						// 設置されていた場所を取得
						Vector2int index = IndexByPosition(characterManager_->GetAllCharactor()[i]->data.transforms.translate);
						// 設置されていた場所を[駒をおける場所]に戻す
						map_->EffectType[index.y][index.x] = BLOCK_EFFECT_TYPE::移動可能;
						// 設置されていた場所を[誰もいない場所]に戻す
						map_->CharactorType[index.y][index.x] = BLOCK_CHAR::Empty;
						// 駒をバトルキャラリストから削除
						characterManager_->SubButtleCharactorList(characterManager_->GetAllCharactor()[i]);
					}
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
						// 設置場所の確定
						if (map_->IsCollisionMouseRay_[y][x] == true)
						{
							// 駒の設置
							characterManager_->GetAllCharactor()[i]->data.transforms.translate = PositionByIndex(Vector2int{ x,y }, map_->BlockTypeByIndex(Vector2int{ x,y }));
							// 駒をバトルキャラリストに追加
							characterManager_->AddButtleCharactorList(characterManager_->GetAllCharactor()[i]);
							// バトルキャラが増える度にアクションディレイ順にソート
							characterManager_->Sort_Buttle_ActionDelay();
							// i 番目のキャラが配置されたことを記録
							isSet[i] = true;
							// 配置した場所にプレイヤーが配置されたことを記録
							map_->CharactorType[y][x] = BLOCK_CHAR::OnPlayer;
							// エフェクトの更新
							map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
							// ここでループを終わらせる
							y = MAP_HEIGHT;
							x = MAP_WIDTH;
						}
						// 設置場所が見つからなかった
						if (y == MAP_HEIGHT - 1 && x == MAP_WIDTH - 1 && isSet[i] == false)
						{
							// アイコンをもとの位置に戻す
							characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.x = 70.0f;
							characterManager_->GetAllCharactor()[i]->dataSeat_type.transforms.translate.y = i * 40.0f + 30.0f;
						}
					}
				}
			}
		}

		// マップとマウスの当たり判定
		for (uint32_t y = 0; y < MAP_HEIGHT; ++y)
		{
			for (uint32_t x = 0; x < MAP_WIDTH; ++x)
			{
				// 色の初期化
				map_->IsCollisionMouseRay_[y][x] = false;
				// 駒をおける場所か判断
				if (map_->BlockEffectByIndex(Vector2int(x, y)) == BLOCK_EFFECT_TYPE::移動可能)
				{
					// まだ色を変えられたマスがない（TheOneがfalse）なら色を変える
					if (IsCollision(Game::GetMouseRay(), map_->data[y][x].AABB))
					{
						// おける場所だったので色を変える
						map_->IsCollisionMouseRay_[y][x] = true;
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
		if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
		{
			finishSetUp = true;
			frame_camera = 0;
			frameMAX_camera = 180;
			Game::MoveCenterTarget({ -11.4f, -0.0f, -4.10f }, frameMAX_camera, EaseType::OUT_QUART);
			Game::MoveRotateTarget({ 1.1f,float(std::numbers::pi) * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
			Game::MoveDistanceTarget(32.40f, frameMAX_camera, EaseType::OUT_QUART);
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
					map_->IsCollisionMouseRay_[y][x] = false;
				}
			}
			for (int i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
			{
				characterManager_->GetButtleCharactor()[i]->pos = IndexByPosition(characterManager_->GetButtleCharactor()[i]->data.transforms.translate);
			}
		}
	}
	else
	{
		for (int i = 0; i < int(characterManager_->GetAllCharactor().size()); ++i)
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
			targetphase_ = GameScenePhase::Check;

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
		// 配置されてなかったらアイコン描画
		if (!isSet[i]) characterManager_->GetAllCharactor()[i]->dataSeat_type.Draw();
	}
	for (uint32_t i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
	{
		characterManager_->GetButtleCharactor()[i]->data.Draw();
	}
}

// 行動キャラがプレイヤーサイドだった時に呼び出される
void GameScene::Initialize_PlayerTurn()
{
	frame_camera = 60;
	frameMAX_camera = 60;
	Act = CharactorSelectPattern::None;
	moveTragetCoolTome = 0;
	ChangeFocus();

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
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.x -= 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
			if (L && moveTargetPositionIndex.x < MAP_WIDTH - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x + 1, moveTargetPositionIndex.y };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.x += 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
				//if (map_->A_to_B(from, to))
				//{
				//	if (map_->shotestCost(IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate), to) <= characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint)
				//	{
				//		moveTargetPositionIndex.x += 1;
				//		moveTragetCoolTome = 0;
				//		characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				//	}
				//}
			}
			if (T && moveTargetPositionIndex.y > 0)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y - 1 };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.y -= 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
			if (B && moveTargetPositionIndex.y < MAP_HEIGHT - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y + 1 };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.y += 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
		}
		if (GetHitKey::keys[DIK_SPACE])
		{
			// 移動してたら
			Vector2int from = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
			Vector2int to = moveTargetPositionIndex;

			if (from != to)
			{
				// 実際に移動
				characterManager_->GetButtleCharactor()[0]->data.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				characterManager_->GetButtleCharactor()[0]->pos = to;
				// 移動コストの取得
				float moveCost = MOVE_COST;
				// 素早さ補正の取得
				float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
				// アクションディレイの更新
				characterManager_->GetButtleCharactor()[0]->actionDelay += moveCost * percentage;
				// マップ更新
				map_->CharactorType[from.y][from.x] = BLOCK_CHAR::Empty;
				map_->CharactorType[to.y][to.x] = BLOCK_CHAR::OnPlayer;
				// フェーズ更新
				targetphase_ = GameScenePhase::Check;
				Act = CharactorSelectPattern::None;
				// 移動可能マスを示すエフェクトをリセット
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						// すでに移動可能になってるやつは初期化
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
						{
							map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
						}
					}
				}
			}
			// 移動先が変わってなかったら
			else
			{
				// フェーズ更新
				targetphase_ = GameScenePhase::Check;
				Act = CharactorSelectPattern::None;
				// 移動可能マスを示すエフェクトをリセット
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						// すでに移動可能になってるやつは初期化
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
						{
							map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
						}
					}
				}
			}
		}
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
			// 移動可能マスを示すエフェクトをリセット
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					// すでに移動可能になってるやつは初期化
					if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
					{
						map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
					}
				}
			}
		}
		moveTragetCoolTome++;
	}

	else if (Act == CharactorSelectPattern::ChangeCameraMode)
	{
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
		}
		if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
		}
	}

	else if (Act == CharactorSelectPattern::Skill)
	{
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;

			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					// すでに移動可能になってるやつは初期化
					map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
				}
			}
		}

		if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)
		{
			if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::直線)
			{
				if (R)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Right;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (L)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Left;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (T)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Up;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (B)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Down;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_LINE(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
			else if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::十字)
			{
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_CROSS(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
			else if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::前方正方形)
			{
				if (R)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Right;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (L)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Left;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (T)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Up;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (B)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Down;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
				}
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_FRONT(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
		}
		else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::バフ)
		{
			if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					for (int y = 0; y < MAP_HEIGHT; ++y)
					{
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::攻撃範囲)
						{
							if (map_->CharactorType[y][x] == BLOCK_CHAR::OnPlayer)
							{
								for (int i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
								{
									if (characterManager_->GetButtleCharactor()[i]->pos == Vector2int{ x,y })
									{
										switch (characterManager_->GetButtleCharactor()[0]->skill.buffTargetStates)
										{
										case AllStates::power:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.power *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											break;
										case AllStates::hp_max:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											break;
										case AllStates::speed:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.speed *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											if (characterManager_->GetButtleCharactor()[i]->states_buttle_.speed >= 100)
											{
												characterManager_->GetButtleCharactor()[i]->states_buttle_.speed = 99;
											}
											break;
										case AllStates::moveRenge:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max += 1;
											break;
										case AllStates::例外:
											break;
										default:
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)
		{
			if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
			{
				Act = CharactorSelectPattern::SkillAnimation;
				SkillAreaShape dd = characterManager_->GetButtleCharactor()[0]->skill.areaShape;
				Initialize_Heal(characterManager_->GetButtleCharactor()[0]->skill.range, BLOCK_CHAR::OnPlayer);
			}
		}
		Update_target();
	}

	else if (Act == CharactorSelectPattern::SkillAnimation)
	{
		Update_target();
		switch (characterManager_->GetButtleCharactor()[0]->skill.areaShape)
		{
		case SkillAreaShape::円:
		{
			if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)Update_AttackEffect_CROSS();
			else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)Update_Heal();
			break;
		}
		case SkillAreaShape::直線:
		{
			Update_AttackEffect_LINE();
			break;
		}
		case SkillAreaShape::十字:
		{
			if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)Update_AttackEffect_CROSS();
			else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)Update_Heal();
			break;
		}
		case SkillAreaShape::正方形:
		{
			if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)Update_AttackEffect_CROSS();
			else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)Update_Heal();
			break;
		}
		case SkillAreaShape::前方正方形:
		{
			Update_AttackEffect_FRONT();
			break;
		}
		case SkillAreaShape::例外:
		{
			break;
		}
		default:
			break;
		}

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


// 行動キャラがエネミーサイドだった時に呼び出される
void GameScene::Initialize_EnemyTurn()
{
	frame_camera = 60;
	frameMAX_camera = 60;
	Act = CharactorSelectPattern::None;
	moveTragetCoolTome = 0;
	ChangeFocus();

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

void GameScene::Update_EnemyTurn()
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
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.x -= 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
			if (L && moveTargetPositionIndex.x < MAP_WIDTH - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x + 1, moveTargetPositionIndex.y };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.x += 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
			if (T && moveTargetPositionIndex.y > 0)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y - 1 };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.y -= 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
			if (B && moveTargetPositionIndex.y < MAP_HEIGHT - 1)
			{
				Vector2int from = moveTargetPositionIndex;
				Vector2int to = Vector2int{ moveTargetPositionIndex.x, moveTargetPositionIndex.y + 1 };
				// 移動できるか
				if (map_->EffectType[to.y][to.x] == BLOCK_EFFECT_TYPE::移動可能)
				{
					moveTargetPositionIndex.y += 1;
					moveTragetCoolTome = 0;
					characterManager_->GetButtleCharactor()[0]->targetdata.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				}
			}
		}
		if (GetHitKey::keys[DIK_SPACE])
		{
			// 移動してたら
			Vector2int from = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
			Vector2int to = moveTargetPositionIndex;

			if (from != to)
			{
				// 実際に移動
				characterManager_->GetButtleCharactor()[0]->data.transforms.translate = PositionByIndex(moveTargetPositionIndex, map_->BlockTypeByIndex(moveTargetPositionIndex));
				characterManager_->GetButtleCharactor()[0]->pos = to;
				// 移動コストの取得
				float moveCost = MOVE_COST;
				// 素早さ補正の取得
				float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
				// アクションディレイの更新
				characterManager_->GetButtleCharactor()[0]->actionDelay += moveCost * percentage;
				// マップ更新
				map_->CharactorType[from.y][from.x] = BLOCK_CHAR::Empty;
				map_->CharactorType[to.y][to.x] = BLOCK_CHAR::OnEnemy;
				// フェーズ更新
				targetphase_ = GameScenePhase::Check;
				Act = CharactorSelectPattern::None;
				// 移動可能マスを示すエフェクトをリセット
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						// すでに移動可能になってるやつは初期化
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
						{
							map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
						}
					}
				}
			}
			// 移動先が変わってなかったら
			else
			{
				// フェーズ更新
				targetphase_ = GameScenePhase::Check;
				Act = CharactorSelectPattern::None;
				// 移動可能マスを示すエフェクトをリセット
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						// すでに移動可能になってるやつは初期化
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
						{
							map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
						}
					}
				}
			}
		}
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
			// 移動可能マスを示すエフェクトをリセット
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					// すでに移動可能になってるやつは初期化
					if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::移動可能)
					{
						map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
					}
				}
			}
		}
		moveTragetCoolTome++;
	}

	else if (Act == CharactorSelectPattern::ChangeCameraMode)
	{
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
		}
		if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;
		}
	}

	else if (Act == CharactorSelectPattern::Skill)
	{
		if (GetHitKey::keys[DIK_Q] && !GetHitKey::preKeys[DIK_Q])
		{
			// フェーズ更新
			targetphase_ = GameScenePhase::Check;
			Act = CharactorSelectPattern::None;

			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					// すでに移動可能になってるやつは初期化
					map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
				}
			}
		}

		if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)
		{
			if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::直線)
			{
				if (R)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Right;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (L)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Left;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (T)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Up;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (B)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Down;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_LINE(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
			else if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::十字)
			{
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_CROSS(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
			else if (characterManager_->GetButtleCharactor()[0]->skill.areaShape == SkillAreaShape::前方正方形)
			{
				if (R)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Right;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (L)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Left;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (T)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Up;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (B)
				{
					characterManager_->GetButtleCharactor()[0]->direction = Direction::Down;
					map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
					Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
				}
				if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
				{
					Act = CharactorSelectPattern::SkillAnimation;
					CheckDamage();
					Initialize_AttackEffect_FRONT(characterManager_->GetButtleCharactor()[0]->skill.range);
				}
			}
		}
		else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::バフ)
		{
			if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					for (int y = 0; y < MAP_HEIGHT; ++y)
					{
						if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::攻撃範囲)
						{
							if (map_->CharactorType[y][x] == BLOCK_CHAR::OnEnemy)
							{
								for (int i = 0; i < characterManager_->GetButtleCharactor().size(); ++i)
								{
									if (characterManager_->GetButtleCharactor()[i]->pos == Vector2int{ x,y })
									{
										switch (characterManager_->GetButtleCharactor()[0]->skill.buffTargetStates)
										{
										case AllStates::power:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.power *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											break;
										case AllStates::hp_max:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											break;
										case AllStates::speed:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.speed *=
												characterManager_->GetButtleCharactor()[0]->skill.multiplier;
											if (characterManager_->GetButtleCharactor()[i]->states_buttle_.speed >= 100)
											{
												characterManager_->GetButtleCharactor()[i]->states_buttle_.speed = 99;
											}
											break;
										case AllStates::moveRenge:
											characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max += 1;
											break;
										case AllStates::例外:
											break;
										default:
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)
		{
			if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
			{
				Act = CharactorSelectPattern::SkillAnimation;
				SkillAreaShape dd = characterManager_->GetButtleCharactor()[0]->skill.areaShape;
				Initialize_Heal(characterManager_->GetButtleCharactor()[0]->skill.range, BLOCK_CHAR::OnEnemy);
			}
		}
		Update_target();
	}

	else if (Act == CharactorSelectPattern::SkillAnimation)
	{
		Update_target();
		switch (characterManager_->GetButtleCharactor()[0]->skill.areaShape)
		{
		case SkillAreaShape::円:
		{

			break;
		}
		case SkillAreaShape::直線:
		{
			Update_AttackEffect_LINE();
			break;
		}
		case SkillAreaShape::十字:
		{
			Update_AttackEffect_CROSS();
			break;
		}
		case SkillAreaShape::正方形:
		{

			break;
		}
		case SkillAreaShape::前方正方形:
		{

			break;
		}
		case SkillAreaShape::例外:
		{
			break;
		}
		default:
			break;
		}

	}
}

void GameScene::Draw_EnemyTurn()
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

// ダメージ判定
void GameScene::CheckDamage()
{
	std::vector<int> deathFrag;

	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::攻撃範囲)
			{
				for (int i = 1; i < characterManager_->GetButtleCharactor().size(); ++i)
				{
					if (characterManager_->GetButtleCharactor()[i]->pos == Vector2int{ x,y })
					{
						characterManager_->GetButtleCharactor()[i]->states_buttle_.hp -=
							characterManager_->GetButtleCharactor()[0]->states_buttle_.power *
							characterManager_->GetButtleCharactor()[0]->skill.multiplier;
						if (characterManager_->GetButtleCharactor()[i]->states_buttle_.hp < 0)
						{
							deathFrag.push_back(i);
							map_->CharactorType[characterManager_->GetButtleCharactor()[i]->pos.y][characterManager_->GetButtleCharactor()[i]->pos.x] = BLOCK_CHAR::Empty;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < deathFrag.size(); ++i)
	{
		characterManager_->eraseButtleCharactor(deathFrag[i]);
	}
}

void GameScene::CheckHeal()
{
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (map_->EffectType[y][x] == BLOCK_EFFECT_TYPE::回復範囲)
			{
				for (int i = 1; i < characterManager_->GetButtleCharactor().size(); ++i)
				{
					if (characterManager_->GetButtleCharactor()[i]->pos == Vector2int{ x,y })
					{
						characterManager_->GetButtleCharactor()[i]->states_buttle_.hp +=
							characterManager_->GetButtleCharactor()[0]->states_buttle_.power *
							characterManager_->GetButtleCharactor()[0]->skill.multiplier;
						if (characterManager_->GetButtleCharactor()[i]->states_buttle_.hp > characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max)
						{
							characterManager_->GetButtleCharactor()[i]->states_buttle_.hp = characterManager_->GetButtleCharactor()[i]->states_buttle_.hp_max;
						}
					}
				}
			}
		}
	}
}

// バトル結果
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

// 視点切り替え(コマンド選択もここ)
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
	// １キャラ注視中(コマンド選択もここ)
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
						Act = CharactorSelectPattern::ChangeCameraMode;
					}
					// 移動
					else if (i == 1)
					{
						ChangeOver();
						Act = CharactorSelectPattern::Move;
						moveTargetPosition = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
						moveTargetPositionIndex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
						characterManager_->GetButtleCharactor()[0]->targetdata = characterManager_->GetButtleCharactor()[0]->data;
						if (characterManager_->GetButtleCharactor()[0]->EnemyOrPlayer_)
						{
							characterManager_->GetButtleCharactor()[0]->targetdata.color = 0xFFFFFF55;
						}
						else
						{
							characterManager_->GetButtleCharactor()[0]->targetdata.color = 0xFF111155;
						}
						map_->CheckAblemovement(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->states_buttle_.movePoint);
					}
					// スキル
					else if (i == 2)
					{
						ChangeOver();
						Act = CharactorSelectPattern::Skill;
						moveTargetPositionIndex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
						map_->CheckAbleAttack(moveTargetPositionIndex, characterManager_->GetButtleCharactor()[0]->skill, characterManager_->GetButtleCharactor()[0]->direction);
						// 使用者がプレイヤー
						if (characterManager_->GetButtleCharactor()[0]->EnemyOrPlayer_)
						{
							// 攻撃スキル
							if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)
							{
								Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnEnemy);
							}
							else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::バフ)
							{
								Inintialize_target(0x00FF7FFF, BLOCK_CHAR::OnPlayer);
							}
							else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)
							{
								Inintialize_target(0x00FF7FFF, BLOCK_CHAR::OnPlayer);
							}
						}
						else
						{
							// 攻撃スキル
							if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::攻撃)
							{
								Inintialize_target(0xf72c2cFF, BLOCK_CHAR::OnPlayer);
							}
							else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::バフ)
							{
								Inintialize_target(0x00FF7FFF, BLOCK_CHAR::OnEnemy);
							}
							else if (characterManager_->GetButtleCharactor()[0]->skill.type == SkillType::回復)
							{
								Inintialize_target(0x00FF7FFF, BLOCK_CHAR::OnEnemy);
							}
						}
					}
					// スキル
					else if (i == 3)
					{
						characterManager_->GetButtleCharactor()[0]->actionDelay += 20;
						// フェーズ更新
						targetphase_ = GameScenePhase::Check;
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
	Game::MoveRotateTarget({ 1.1f,float(std::numbers::pi) * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(32.40f, frameMAX_camera, EaseType::OUT_QUART);
}

void GameScene::ChangeFocus()
{
	frame_camera = 0;
	frameMAX_camera = 40;
	cameraMode = CameraMode_Over_or_Focus::Focus;
	Game::MoveCenterTarget(characterManager_->GetButtleCharactor()[0]->data.transforms.translate, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 0.8f,float(std::numbers::pi) * 2.0f, 0.0f }, frameMAX_camera, EaseType::OUT_QUART);
	Game::MoveDistanceTarget(15.60f, frameMAX_camera, EaseType::OUT_QUART);
}


// 攻撃ターゲット
void GameScene::Inintialize_target(int color, BLOCK_CHAR targetSide)
{
	AttackTaregtEffectData.clear();

	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (map_->EffectType[y][x] != BLOCK_EFFECT_TYPE::Empty)
			{
				if (map_->CharactorType[y][x] == targetSide)
				{
					Game::RenderData_Model target;
					target.model = uint32_t(TEXTURE::Attack_Marker);
					target.texture = uint32_t(TEXTURE::Attack_Marker);
					target.transforms.translate = PositionByIndex(Vector2int{ x,y }, map_->BlockTypeByIndex(Vector2int{ x,y }));
					target.transforms.translate.y += 2.5f;
					target.color = color;
					AttackTaregtEffectData.push_back(target);
				}
			}
		}
	}

	targetEffectFrame = 0;
}

void GameScene::Update_target()
{
	for (int i = 0; i < AttackTaregtEffectData.size(); ++i)
	{
		AttackTaregtEffectData[i].transforms.translate.y += (std::sinf(float(targetEffectFrame) / 10.0f)) / 80.0f;
		AttackTaregtEffectData[i].transforms.rotate.y += 0.1f;
		AttackTaregtEffectData[i].Draw();
	}
	targetEffectFrame++;
}


// 攻撃エフェクト
void GameScene::Initialize_AttackEffect_LINE(int range)
{
	AttacEffectsikakusuiData.clear();

	for (int i = 0; i < 30; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.1f, 0.1f };

		Direction dir = characterManager_->GetButtleCharactor()[0]->direction;
		switch (dir)
		{
		case Direction::None:
			break;
		case Direction::Left:
			add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
			add.velocity = { -0.5f,0.0f,0.0f };
			add.targetindex.x += range;
			break;
		case Direction::Right:
			add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
			add.velocity = { 0.5f,0.0f,0.0f };
			add.targetindex.x -= range;
			break;
		case Direction::Down:
			add.data.transforms.rotate.y = float(std::numbers::pi);
			add.velocity = { 0.0f,0.0f,-0.5f };
			add.targetindex.y += range;
			break;
		case Direction::Up:
			add.data.transforms.rotate.y = float(std::numbers::pi);
			add.velocity = { 0.0f,0.0f,0.5f };
			add.targetindex.y -= range;
			break;
		default:
			break;
		}

		add.frame = i * 2;

		AttacEffectsikakusuiData.push_back(add);
	}

	Animationt = 0;

}

void GameScene::Update_AttackEffect_LINE()
{
	for (int i = 0; i < 30; ++i)
	{
		if (Animationt > AttacEffectsikakusuiData[i].frame)
		{
			AttacEffectsikakusuiData[i].data.transforms.translate += AttacEffectsikakusuiData[i].velocity;
			AttacEffectsikakusuiData[i].data.transforms.rotate.z += 0.1f;
			AttacEffectsikakusuiData[i].data.Draw();
			if (AttacEffectsikakusuiData[i].starttype == BLOCK_TYPE::Empty)
			{
				Vector2int imdex = IndexByPosition(AttacEffectsikakusuiData[i].data.transforms.translate);
				if (map_->BlockTypeByIndex(imdex) == BLOCK_TYPE::Wall)
				{
					AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
				}
				if (imdex == AttacEffectsikakusuiData[i].targetindex)
				{
					if (AttacEffectsikakusuiData[i].data.transforms.scale.x > 0)
					{
						AttacEffectsikakusuiData[i].data.transforms.scale -= { 0.05f, 0.05f, 0.05f };
						if (AttacEffectsikakusuiData[i].data.transforms.scale.x <= 0)
						{
							AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
						}
					}
				}
			}
			else if (AttacEffectsikakusuiData[i].starttype == BLOCK_TYPE::Wall)
			{
				Vector2int imdex = IndexByPosition(AttacEffectsikakusuiData[i].data.transforms.translate);
				if (imdex == AttacEffectsikakusuiData[i].targetindex)
				{
					if (AttacEffectsikakusuiData[i].data.transforms.scale.x > 0)
					{
						AttacEffectsikakusuiData[i].data.transforms.scale -= { 0.05f, 0.05f, 0.05f };
						if (AttacEffectsikakusuiData[i].data.transforms.scale.x <= 0)
						{
							AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
						}
					}
				}
			}
		}
	}

	if (Animationt > 60)
	{
		// 攻撃コストの取得
		float attackCost = 20;
		// 素早さ補正の取得
		float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
		// アクションディレイの更新
		characterManager_->GetButtleCharactor()[0]->actionDelay += attackCost * percentage;
		// フェーズ更新
		targetphase_ = GameScenePhase::Check;
		// 移動可能マスを示すエフェクトをリセット
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				// すでに移動可能になってるやつは初期化
				map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
			}
		}
	}

	Animationt++;
}

void GameScene::Initialize_AttackEffect_CROSS(int range)
{
	AttacEffectsikakusuiData.clear();

	for (int i = 0; i < 10; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.1f, 0.1f };

		add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
		add.velocity = { -0.5f,0.0f,0.0f };
		add.targetindex.x += range;

		add.frame = i * 2;

		AttacEffectsikakusuiData.push_back(add);
	}
	for (int i = 0; i < 10; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.1f, 0.1f };

		add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
		add.velocity = { 0.5f,0.0f,0.0f };
		add.targetindex.x -= range;

		add.frame = i * 2;

		AttacEffectsikakusuiData.push_back(add);
	}
	for (int i = 0; i < 10; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.1f, 0.1f };

		add.data.transforms.rotate.y = float(std::numbers::pi);
		add.velocity = { 0.0f,0.0f,-0.5f };
		add.targetindex.y += range;

		add.frame = i * 2;

		AttacEffectsikakusuiData.push_back(add);
	}
	for (int i = 0; i < 10; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.1f, 0.1f };

		add.data.transforms.rotate.y = float(std::numbers::pi);
		add.velocity = { 0.0f,0.0f,0.5f };
		add.targetindex.y -= range;

		add.frame = i * 2;

		AttacEffectsikakusuiData.push_back(add);
	}

	Animationt = 0;
}

void GameScene::Update_AttackEffect_CROSS()
{
	for (int i = 0; i < 40; ++i)
	{
		if (Animationt > AttacEffectsikakusuiData[i].frame)
		{
			AttacEffectsikakusuiData[i].data.transforms.translate += AttacEffectsikakusuiData[i].velocity;
			AttacEffectsikakusuiData[i].data.transforms.rotate.z += 0.2f;
			AttacEffectsikakusuiData[i].data.Draw();
			if (AttacEffectsikakusuiData[i].starttype == BLOCK_TYPE::Empty)
			{
				Vector2int imdex = IndexByPosition(AttacEffectsikakusuiData[i].data.transforms.translate);
				if (map_->BlockTypeByIndex(imdex) == BLOCK_TYPE::Wall)
				{
					AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
				}
				if (imdex == AttacEffectsikakusuiData[i].targetindex)
				{
					if (AttacEffectsikakusuiData[i].data.transforms.scale.x > 0)
					{
						AttacEffectsikakusuiData[i].data.transforms.scale -= { 0.05f, 0.05f, 0.05f };
						if (AttacEffectsikakusuiData[i].data.transforms.scale.x <= 0)
						{
							AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
						}
					}
				}
			}
			else if (AttacEffectsikakusuiData[i].starttype == BLOCK_TYPE::Wall)
			{
				Vector2int imdex = IndexByPosition(AttacEffectsikakusuiData[i].data.transforms.translate);
				if (imdex == AttacEffectsikakusuiData[i].targetindex)
				{
					if (AttacEffectsikakusuiData[i].data.transforms.scale.x > 0)
					{
						AttacEffectsikakusuiData[i].data.transforms.scale -= { 0.05f, 0.05f, 0.05f };
						if (AttacEffectsikakusuiData[i].data.transforms.scale.x <= 0)
						{
							AttacEffectsikakusuiData[i].data.transforms.scale = { 0.0f, 0.0f, 0.0f };
						}
					}
				}
			}
		}
	}



	if (Animationt > 60)
	{
		// 攻撃コストの取得
		float attackCost = 20;
		// 素早さ補正の取得
		float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
		// アクションディレイの更新
		characterManager_->GetButtleCharactor()[0]->actionDelay += attackCost * percentage;
		// フェーズ更新
		targetphase_ = GameScenePhase::Check;
		// 移動可能マスを示すエフェクトをリセット
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				// すでに移動可能になってるやつは初期化
				map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
			}
		}
	}

	Animationt++;
}

void GameScene::Initialize_AttackEffect_FRONT(int range)
{
	AttacEffectsikakusuiData.clear();

	for (int i = 0; i < 20; ++i)
	{
		dataaaaaa add;

		add.data.model = uint32_t(TEXTURE::Attack_Effect);
		add.data.texture = uint32_t(TEXTURE::Attack_Effect);
		add.data.options.enableLighting = false;
		add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
		add.targetindex = add.startindex;
		add.starttype = map_->BlockTypeByIndex(add.startindex);

		add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
		add.data.transforms.translate.y += 0.5f;
		add.data.transforms.translate.x += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.y += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.translate.z += float(RandomInt(-30, 30)) / 100.0f;
		add.data.transforms.scale = { 0.1f, 0.05f, 0.1f };

		add.range = range;

		Direction dir = characterManager_->GetButtleCharactor()[0]->direction;
		switch (dir)
		{
		case Direction::None:
			break;
		case Direction::Left:
			add.targetindex.x += range;
			add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
			add.velocity = { -0.5f,float(RandomInt(-40, 40)) / 100.0f,float(RandomInt(-40, 40)) / 100.0f };
			break;
		case Direction::Right:
			add.targetindex.x -= range;
			add.data.transforms.rotate.y = float(std::numbers::pi / 2.0);
			add.velocity = { 0.5f,float(RandomInt(-40, 40)) / 100.0f,float(RandomInt(-40, 40)) / 100.0f };
			break;
		case Direction::Down:
			add.targetindex.y += range;
			add.data.transforms.rotate.y = float(std::numbers::pi);
			add.velocity = { float(RandomInt(-40, 40)) / 100.0f,float(RandomInt(-40, 40)) / 100.0f,-0.5f };
			break;
		case Direction::Up:
			add.targetindex.y -= range;
			add.data.transforms.rotate.y = float(std::numbers::pi);
			add.velocity = { float(RandomInt(-40, 40)) / 100.0f,float(RandomInt(-40, 40)) / 100.0f,0.5f };
			break;
		default:
			break;
		}

		add.velocity *= 0.8f;

		add.frame = 0;

		AttacEffectsikakusuiData.push_back(add);
	}

	Animationt = 0;
}

void GameScene::Update_AttackEffect_FRONT()
{
	for (int i = 0; i < 20; ++i)
	{
		if (Animationt > AttacEffectsikakusuiData[i].frame)
		{
			AttacEffectsikakusuiData[i].data.transforms.translate += AttacEffectsikakusuiData[i].velocity;
			AttacEffectsikakusuiData[i].data.transforms.rotate.z += 0.1f;
			AttacEffectsikakusuiData[i].data.Draw();

			Vector2int imdex = IndexByPosition(AttacEffectsikakusuiData[i].data.transforms.translate);
			if (map_->BlockEffectByIndex(imdex) != BLOCK_EFFECT_TYPE::攻撃範囲)
			{
				AABB aabb;
				Vector3 aabbcenter = PositionByIndex(AttacEffectsikakusuiData[i].targetindex);

				float extent = AttacEffectsikakusuiData[i].range * 1.2f;
				Vector3 halfExtent(extent * 0.5f);

				// AABBの構築
				aabb.min = aabbcenter - halfExtent;
				aabb.max = aabbcenter + halfExtent;

				Vector3& position = AttacEffectsikakusuiData[i].data.transforms.translate;
				Vector3& velocity = AttacEffectsikakusuiData[i].velocity;

				// AABBの範囲チェックと反射処理
				if (position.x < aabb.min.x || position.x > aabb.max.x)
				{
					velocity.x *= -1.0f;
					// はみ出し補正（オプション）
					position.x = std::clamp(position.x, aabb.min.x, aabb.max.x);
				}

				if (position.y < aabb.min.y || position.y > aabb.max.y)
				{
					velocity.y *= -1.0f;
					position.y = std::clamp(position.y, aabb.min.y, aabb.max.y);
				}

				if (position.z < aabb.min.z || position.z > aabb.max.z)
				{
					velocity.z *= -1.0f;
					position.z = std::clamp(position.z, aabb.min.z, aabb.max.z);
				}
			}
		}
	}

	if (Animationt > 60)
	{
		// 攻撃コストの取得
		float attackCost = 20;
		// 素早さ補正の取得
		float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
		// アクションディレイの更新
		characterManager_->GetButtleCharactor()[0]->actionDelay += attackCost * percentage;
		// フェーズ更新
		targetphase_ = GameScenePhase::Check;
		// 移動可能マスを示すエフェクトをリセット
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				// すでに移動可能になってるやつは初期化
				map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
			}
		}
	}

	Animationt++;
}

void GameScene::Initialize_Heal(int range, BLOCK_CHAR targetSide)
{
	AttacEffectsikakusuiData.clear();

	for (int x = 0; x < MAP_WIDTH; ++x)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			if (map_->EffectType[y][x] != BLOCK_EFFECT_TYPE::Empty)
			{
				if (map_->CharactorType[y][x] == targetSide)
				{
					for (int i = 0; i < 30; ++i)
					{
						dataaaaaa add;

						add.data.model = uint32_t(TEXTURE::Attack_Effect);
						add.data.texture = uint32_t(TEXTURE::Attack_Effect);
						add.data.options.enableLighting = false;
						add.startindex = IndexByPosition(characterManager_->GetButtleCharactor()[0]->data.transforms.translate);
						add.targetindex = add.startindex;
						add.starttype = map_->BlockTypeByIndex(add.startindex);

						//add.data.transforms.translate = characterManager_->GetButtleCharactor()[0]->data.transforms.translate;
						add.data.transforms.translate = PositionByIndex(Vector2int{ x,y }, map_->BlockTypeByIndex(Vector2int{ x,y }));
						add.data.transforms.translate.x += float(RandomInt(-50, 50)) / 100.0f;
						add.data.transforms.translate.y += float(RandomInt(-50, 50)) / 100.0f;
						add.data.transforms.translate.z += float(RandomInt(-50, 50)) / 100.0f;
						add.data.transforms.rotate.x = float(std::numbers::pi / 2.0f);
						add.data.transforms.scale = { 0.2f, 0.2f, 0.2f };

						add.velocity = { 0.0f,0.1f,0.0f };

						add.frame = i * 2;

						AttacEffectsikakusuiData.push_back(add);
					}
				}
			}
		}
	}

	CheckHeal();
	Animationt = 0;
}

void GameScene::Update_Heal()
{
	for (int i = 0; i < int(AttacEffectsikakusuiData.size()); ++i)
	{
		if (Animationt > AttacEffectsikakusuiData[i].frame)
		{
			AttacEffectsikakusuiData[i].data.transforms.translate += AttacEffectsikakusuiData[i].velocity;
			AttacEffectsikakusuiData[i].data.transforms.rotate.z += 0.1f;
			if (AttacEffectsikakusuiData[i].data.transforms.scale.x > 0)
			{
				AttacEffectsikakusuiData[i].data.transforms.scale -= { 0.01f, 0.01f, 0.01f };
			}
			AttacEffectsikakusuiData[i].data.Draw();
		}
	}

	if (Animationt > 60)
	{
		// 攻撃コストの取得
		float attackCost = 20;
		// 素早さ補正の取得
		float percentage = float(characterManager_->GetButtleCharactor()[0]->states_buttle_.speed) / 100;
		// アクションディレイの更新
		characterManager_->GetButtleCharactor()[0]->actionDelay += attackCost * percentage;
		// フェーズ更新
		targetphase_ = GameScenePhase::Check;
		// 移動可能マスを示すエフェクトをリセット
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				// すでに移動可能になってるやつは初期化
				map_->EffectType[y][x] = BLOCK_EFFECT_TYPE::Empty;
			}
		}
	}

	Animationt++;
}







