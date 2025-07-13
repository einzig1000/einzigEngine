#include "TR2Class.h"
#include <format>

TR2Class::TR2Class()
	:gCostMap(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, (std::numeric_limits<int>::max)()))
{
	SkillInfo skill;

	skill.cost = 12;
	skill.delayCost = 0;
	skill.range = 5;
	skill.skillName = "すきる１";
	skill.skillOdds = 1.2f;
	skill.skillType = SkillType::Attack;
	skillList.push_back(skill);

	skill.cost = 16;
	skill.delayCost = 0;
	skill.range = 2;
	skill.skillName = "すきる２";
	skill.skillOdds = 1.6f;
	skill.skillType = SkillType::Attack;
	skillList.push_back(skill);

	SkillInfo advancedSkill;

	advancedSkill.cost = 40;
	advancedSkill.delayCost = 0;
	advancedSkill.range = 3;
	advancedSkill.skillName = "つよすきる１";
	advancedSkill.skillOdds = 3.0f;
	advancedSkill.skillType = SkillType::Attack;
	advancedSkillList.push_back(advancedSkill);

	advancedSkill.cost = 4;
	advancedSkill.delayCost = 0;
	advancedSkill.range = 1;
	advancedSkill.skillName = "つよすきる２";
	advancedSkill.skillOdds = 10.0f;
	advancedSkill.skillType = SkillType::Attack;
	advancedSkillList.push_back(advancedSkill);

	SkillInfo ultimateSkill;

	ultimateSkill.cost = 120;
	ultimateSkill.delayCost = 100;
	ultimateSkill.range = 15;
	ultimateSkill.skillName = "うると１";
	ultimateSkill.skillOdds = 50.0f;
	ultimateSkill.skillType = SkillType::Attack;
	ultimateSkillList.push_back(ultimateSkill);

	ultimateSkill.cost = 10;
	ultimateSkill.delayCost = 0;
	ultimateSkill.range = 15;
	ultimateSkill.skillName = "うると２";
	ultimateSkill.skillOdds = 1.0f;
	ultimateSkill.skillType = SkillType::Attack;
	ultimateSkillList.push_back(ultimateSkill);
}

void TR2Class::Initialize()
{
	/*////////////////////////////////////////
					プレイヤー
	////////////////////////////////////////*/
	player_.model = playerModel;
	player_.texture = uvCheckerPng;
	player_.transforms.scale = { 0.5f, 0.5f, 0.5f };
	player_.transforms.translate = IndexToPosition({ 5,5 });
	player_.transforms.rotate = { 0.0f,0.0f,0.0f };
	player_.pivot = { 0,0,0 };
	player_.color = 0xFFFFFFFF;
	player_.moveRange = 5;

	/*////////////////////////////////////////
					エネミー
	////////////////////////////////////////*/
	// 見た目
	enemy_.model = playerModel;
	enemy_.texture = uvCheckerPng;
	// 描画用
	enemy_.transforms.scale = { 0.5f, 0.5f, 0.5f };
	enemy_.transforms.translate = IndexToPosition({ 10,10 });
	enemy_.transforms.rotate = { 0.0f,0.0f,0.0f };
	enemy_.pivot = { 0,0,0 };
	enemy_.color = 0xFFFFFFFF;
	// 戦闘判断用
	enemy_.HP = 100;
	enemy_.Attack = 10;
	enemy_.moveRange = 4;	// 移動可能範囲
	enemy_.advantagePosition = Environment::高台;	// 自身にとっての有利ポジ（※伴ったスキル構成にすること）
	enemy_.skill = 0;			// スキル識別用　いずれは
	enemy_.advancedSkill = 0;	// スキル識別用　スキルの名前で
	enemy_.passiveSkill = 0;	// スキル識別用　設定できるように
	enemy_.ultimateSkill = 0;	// スキル識別用　変更
	enemy_.delayCost = ultimateSkillList[enemy_.ultimateSkill].delayCost;// ウルト使用制限
	enemy_.priority.MoveToAdvantage = 1.6f;
	enemy_.priority.MoveToEnemy = 1.8f;
	enemy_.priority.Skill = 1.0f;
	enemy_.priority.AdvancedSkill = 1.5f;
	enemy_.priority.UltimateSkill = 2.0f;
	enemy_.priorityReset();

	// ブロック
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].model = blockModel;
			block[y][x].texture = blockPng;
			block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
			block[y][x].transforms.translate = IndexToPosition({ x,y });
			block[y][x].transforms.rotate = { 0.0f,0.0f,0.0f };
			block[y][x].pivot = { 0,0,0 };
			block[y][x].color = 0xFFFFFFFF;
			block[y][x].AABB = Game::CreateAABB(block[y][x].transforms, block[y][x].model);
			block[y][x].cost = 0;
			block[y][x].type = BlockType::Empty;
		}
	}

	// 天球
	skyDome.transforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDome.transforms.translate = { 0.0f,0.0f,0.0f };
	skyDome.transforms.rotate = { 0.0f,0.0f,0.0f };
	skyDome.options.enableWireframeMode = false;

	// 移動中ではない
	movement = false;
	// 移動得イージング用の媒介変数
	movementT = 0;
	// 移動方向
	direction = Direction::None;

	// ターン
	turn = Turn::None;
	turnRepuest = Turn::Player;

	// マップ編集モード
	editMode = true;
	selectBlock = 0;
	PrePressMouse = false;
}

void TR2Class::Update()
{
	// ターン切り替え時の初期化
	if (turnRepuest != Turn::None)
	{
		// ターン変更
		turn = turnRepuest;
		switch (turn)
		{
		case Turn::Player:
			Initialize_PlayerTurn();
			break;
		case Turn::Enemy:
			Initialize_EnemyTurn();
			break;
		case Turn::EnemyConsider:
			Initialize_EnemyConsiderTurn();
			break;
		default:
			break;
		}
		// ターンリクエストリセット
		turnRepuest = Turn::None;
	}

	// ターンごとの更新
	switch (turn)
	{
	case Turn::Player:
		Update_PlayerTurn();
		break;
	case Turn::Enemy:
		Update_EnemyTurn();
		break;
	case Turn::EnemyConsider:
		Update_EnemyConsiderTurn();
		break;
	default:
		break;
	}

	// マップ編集
	if (editMode)
	{
		// ブロックAABB
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				block[y][x].AABB = Game::CreateAABB(block[y][x].transforms, block[y][x].model);
			}
		}
		// ブロックとマウス
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (Game::IsCollisionMouseRayAABB(block[y][x].AABB, block[y][x].model))
				{
					block[y][x].mouseTarget = true;
				}
				else
				{
					block[y][x].mouseTarget = false;
				}
			}
		}
		// ブロック変更
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].mouseTarget && Game::IsPressMouse(0))
				{
					if (selectBlock == 0 && !block[y][x].changeFlag)
					{
						block[y][x].changeFlag = true;
						block[y][x].type = BlockType::Empty;
						block[y][x].transforms.scale.x = 1.0f;
						block[y][x].transforms.scale.y = 1.0f;
						block[y][x].transforms.scale.z = 1.0f;
						block[y][x].transforms.translate.y = 0.0f;
						block[y][x].cost = 0;
					}
					else if (selectBlock == 1 && !block[y][x].changeFlag)
					{
						block[y][x].changeFlag = true;
						block[y][x].type = BlockType::Wall;
						//block[y][x].transforms.scale.x = 1.0f;
						//block[y][x].transforms.scale.y = 5.0f;
						//block[y][x].transforms.scale.z = 1.0f;
						block[y][x].transforms.translate.y = 1.0f;
					}
					else if (selectBlock == 2 && !block[y][x].changeFlag)
					{
						block[y][x].changeFlag = true;
						block[y][x].type = BlockType::Slope;
						//block[y][x].transforms.scale.x = 1.0f;
						//block[y][x].transforms.scale.y = 2.5f;
						//block[y][x].transforms.scale.z = 1.0f;
						block[y][x].transforms.translate.y = 0.5f;
						block[y][x].cost = 0;
					}
					else if (selectBlock == 3 && !block[y][x].changeFlag)
					{
						for (int y = 0; y < MAP_HEIGHT; ++y)
						{
							for (int x = 0; x < MAP_WIDTH; ++x)
							{
								if (block[y][x].type == BlockType::WarpIn)
								{
									block[y][x].type = BlockType::Empty;
									break;
								}
							}
						}
						block[y][x].changeFlag = true;
						block[y][x].type = BlockType::WarpIn;
						block[y][x].transforms.scale.x = 1.0f;
						block[y][x].transforms.scale.y = 1.0f;
						block[y][x].transforms.scale.z = 1.0f;
						block[y][x].transforms.translate.y = 0.0f;
						block[y][x].cost = 0;
					}
					else if (selectBlock == 4 && !block[y][x].changeFlag)
					{
						for (int y = 0; y < MAP_HEIGHT; ++y)
						{
							for (int x = 0; x < MAP_WIDTH; ++x)
							{
								if (block[y][x].type == BlockType::WarpOut)
								{
									block[y][x].type = BlockType::Empty;
									break;
								}
							}
						}
						block[y][x].changeFlag = true;
						block[y][x].type = BlockType::WarpOut;
						block[y][x].transforms.scale.x = 1.0f;
						block[y][x].transforms.scale.y = 1.0f;
						block[y][x].transforms.scale.z = 1.0f;
						block[y][x].transforms.translate.y = 0.0f;
						block[y][x].cost = 0;
					}
				}
			}
		}

		if (!Game::IsPressMouse(0) && PrePressMouse)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					block[y][x].changeFlag = false;
				}
			}
		}
	}




	if (GetHitKey::keys[DIK_3] && !GetHitKey::preKeys[DIK_3])
	{
		Game::toggleWireframeMode();
	}

	skyDome.options.uvTransform.translate.y += 0.001f;
	skyDome.options.uvTransform.translate.x += 0.0001f;
}

void TR2Class::Draw()
{
	// 天球
	skyDome.Draw();

	// ブロック
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (block[y][x].mouseTarget)
			{
				block[y][x].color = 0xece99eFF; // マウスホバー色
			}
			else if (block[y][x].type == BlockType::Empty)
			{
				block[y][x].color = 0xFFFFFFFF; // 白
			}
			else if (block[y][x].type == BlockType::Wall)
			{
				block[y][x].color = 0x6B6B6BFF; // グレー
			}
			else if (block[y][x].type == BlockType::Slope)
			{
				block[y][x].color = 0x76428aFF; // 毒
			}
			else if (block[y][x].type == BlockType::WarpIn || block[y][x].type == BlockType::WarpOut)
			{
				block[y][x].color = 0xFFFFFFFF; // ワープ
			}

			if (block[y][x].state == AstarBlockState::Explored)
			{
				block[y][x].color = 0xffab53FF; // オレンジ（探索済み）
			}
			else if (block[y][x].state == AstarBlockState::Path)
			{
				block[y][x].color = 0x00FF00FF; // 緑（経路）
			}
			else if (block[y][x].state == AstarBlockState::Frontier)
			{
				block[y][x].color = 0x0000FFFF; // 青（探索待ち）
			}

			block[y][x].Draw();
		}
	}

	// プレイヤー
	player_.Draw();

	// エネミー
	enemy_.Draw();

	// ブロックオーラ
	//Game::Drawobj({ {1,1,1},{0.0f,0.0f,3.141592f},block[8][8].transforms.translate }, { 0,0,0 }, blockParticleModel, blockParticlePng, 0xFFFFFFFF);

	switch (turn)
	{
	case Turn::Player:
		Draw_PlayerTurn();
		break;
	case Turn::Enemy:
		Draw_EnemyTurn();
		break;
	case Turn::EnemyConsider:
		Draw_EnemyConsiderTurn();
		break;
	default:
		break;
	}

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("R:Map&Camera Reset");
	ImGui::Checkbox("mapEditMode", &editMode);

	const TextureData* tex{};
	
	if (selectBlock == 0)tex = Game::GetTexture(SelectemptyPng);
	else tex = Game::GetTexture(emptyPng);
	ImGui::Image(static_cast<ImTextureID>(tex->textureSrvHandleGPU.ptr), ImVec2(32, 32));
	if (ImGui::IsItemClicked())selectBlock = 0;
	ImGui::SameLine();

	if (selectBlock == 1)tex = Game::GetTexture(SelectwallPng);
	else tex = Game::GetTexture(wallPng);
	ImGui::Image(static_cast<ImTextureID>(tex->textureSrvHandleGPU.ptr), ImVec2(32, 32));
	if (ImGui::IsItemClicked())selectBlock = 1;
	ImGui::SameLine();

	if (selectBlock == 2)tex = Game::GetTexture(SelectasidPng);
	else tex = Game::GetTexture(asidPng);
	ImGui::Image(static_cast<ImTextureID>(tex->textureSrvHandleGPU.ptr), ImVec2(32, 32));
	if (ImGui::IsItemClicked())selectBlock = 2;
	ImGui::SameLine();

	if (selectBlock == 3)tex = Game::GetTexture(SelectwarpInPng);
	else tex = Game::GetTexture(warpInPng);
	ImGui::Image(static_cast<ImTextureID>(tex->textureSrvHandleGPU.ptr), ImVec2(32, 32));
	if (ImGui::IsItemClicked())selectBlock = 3;
	ImGui::SameLine();

	if (selectBlock == 4)tex = Game::GetTexture(SelectwarpOutPng);
	else tex = Game::GetTexture(warpOutPng);
	ImGui::Image(static_cast<ImTextureID>(tex->textureSrvHandleGPU.ptr), ImVec2(32, 32));
	if (ImGui::IsItemClicked())selectBlock = 4;
	ImGui::SameLine();
}

void TR2Class::Initialize_PlayerTurn()
{
	// ブロック演出ポイントのリセット
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].isAttackTarget = false;
			block[y][x].isBuffTarget = false;
			block[y][x].isHealTarget = false;
			block[y][x].state = AstarBlockState::None; // A*表示のリセット
		}
	}

	// 移動目標をプレイヤーが現在いる場所で初期化
	playerSelectedMoveTarget = PositionToIndex(player_.transforms.translate);
}


void TR2Class::Update_PlayerTurn()
{
	Vector2int candidateTarget = playerSelectedMoveTarget; // プレイヤーの入力による一時的な候補


	if (GetHitKey::keys[DIK_S] && !GetHitKey::preKeys[DIK_S] && !movement)
	{
		Vector2int nextCandidate = candidateTarget;
		nextCandidate.y += 1;
		if (GetShortestPathLength(player_.transforms.translate, IndexToPosition(nextCandidate)) <= player_.moveRange)
		{
			playerSelectedMoveTarget = nextCandidate;
		}
	}
	if (GetHitKey::keys[DIK_W] && !GetHitKey::preKeys[DIK_W] && !movement)
	{
		Vector2int nextCandidate = candidateTarget;
		nextCandidate.y -= 1;
		if (GetShortestPathLength(player_.transforms.translate, IndexToPosition(nextCandidate)) <= player_.moveRange)
		{
			playerSelectedMoveTarget = nextCandidate;
		}
	}
	if (GetHitKey::keys[DIK_A] && !GetHitKey::preKeys[DIK_A] && !movement)
	{
		Vector2int nextCandidate = candidateTarget;
		nextCandidate.x += 1;
		if (GetShortestPathLength(player_.transforms.translate, IndexToPosition(nextCandidate)) <= player_.moveRange)
		{
			playerSelectedMoveTarget = nextCandidate;
		}
	}
	if (GetHitKey::keys[DIK_D] && !GetHitKey::preKeys[DIK_D] && !movement)
	{
		Vector2int nextCandidate = candidateTarget;
		nextCandidate.x -= 1;
		if (GetShortestPathLength(player_.transforms.translate, IndexToPosition(nextCandidate)) <= player_.moveRange)
		{
			playerSelectedMoveTarget = nextCandidate;
		}
	}

	if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE] && !movement)
	{
		// プレイヤーの現在の位置から選択された移動目標地点までのパスを計算
		Astar(player_.transforms.translate, IndexToPosition(playerSelectedMoveTarget));

		if (!pathNodes.empty()) // パスが見つかった場合
		{
			movement = true;
			pathStepIndex = 0;
			// 最初の移動ステップの開始点と終了点を設定
			astarStartIndex = PositionToIndex(player_.transforms.translate); // A*で計算されたパスの最初のノード（現地点）
			astarTargetIndex = pathNodes[pathStepIndex]; // パスの次のノード

			// アニメーション用のTransform情報を設定
			fromBlockTransforms = block[astarStartIndex.y][astarStartIndex.x].transforms;
			nextBlockTransforms = block[astarTargetIndex.y][astarTargetIndex.x].transforms;

			// 移動方向に応じてpivotを調整 (キャラクターの向き用)
			block[astarTargetIndex.y][astarTargetIndex.x].pivot = { 0,0,0 }; // 次のブロックのpivotを初期化
			if (astarStartIndex.x < astarTargetIndex.x)
			{
				direction = Direction::Right;
				player_.pivot.x = -BLOCK_WIDTH / 2.0f; // キャラクターのpivotを調整
			}
			else if (astarStartIndex.x > astarTargetIndex.x)
			{
				direction = Direction::Left;
				player_.pivot.x = BLOCK_WIDTH / 2.0f;
			}
			else if (astarStartIndex.y < astarTargetIndex.y)
			{
				direction = Direction::Down;
				player_.pivot.z = -BLOCK_HEIGHT / 2.0f;
			}
			else if (astarStartIndex.y > astarTargetIndex.y)
			{
				direction = Direction::Up;
				player_.pivot.z = BLOCK_HEIGHT / 2.0f;
			}
		}
	}

	// 移動アニメーション中
	if (movement)
	{
		if (translateBlock(CHARACTER_MOVE_EASING_TIME, player_.transforms))
		{
			// 1ステップの移動が完了
			astarStartIndex = astarTargetIndex; // 現在の終点が次の開始点になる
			pathStepIndex++;
			if (pathStepIndex < pathNodes.size())
			{
				// 次の移動ステップを設定
				astarTargetIndex = pathNodes[pathStepIndex];

				// アニメーション用のTransform情報を更新
				fromBlockTransforms = block[astarStartIndex.y][astarStartIndex.x].transforms;
				nextBlockTransforms = block[astarTargetIndex.y][astarTargetIndex.x].transforms;

				// 移動方向に応じてpivotを調整
				player_.pivot = { 0,0,0 }; // pivotをリセットしてから設定
				if (astarStartIndex.x < astarTargetIndex.x)
				{
					direction = Direction::Right;
					player_.pivot.x = -BLOCK_WIDTH / 2.0f;
				}
				else if (astarStartIndex.x > astarTargetIndex.x)
				{
					direction = Direction::Left;
					player_.pivot.x = BLOCK_WIDTH / 2.0f;
				}
				else if (astarStartIndex.y < astarTargetIndex.y)
				{
					direction = Direction::Down;
					player_.pivot.z = -BLOCK_HEIGHT / 2.0f;
				}
				else if (astarStartIndex.y > astarTargetIndex.y)
				{
					direction = Direction::Up;
					player_.pivot.z = BLOCK_HEIGHT / 2.0f;
				}
			}
			else
			{
				// ゴール到達 (全ステップ完了)
				movement = false;
				pathNodes.clear();
				player_.pivot = { 0,0,0 }; // 移動終了後のpivotリセット
				turnRepuest = Turn::EnemyConsider; // 敵の検討ターンへ
			}
		}
	}
}

void TR2Class::Draw_PlayerTurn()
{
	Game::Drawobj(block[playerSelectedMoveTarget.y][playerSelectedMoveTarget.x].transforms,
		player_.pivot, player_.model, player_.texture, 0xFFFFFF55, player_.options);
	ImGui::Text("[playerSelectedMoveTarget.x = %d][playerSelectedMoveTarget.y = %d]",
		playerSelectedMoveTarget.x, playerSelectedMoveTarget.y);
}


void TR2Class::Initialize_EnemyConsiderTurn()
{
	// ブロック演出ポイントのリセット
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].isAttackTarget = false;
			block[y][x].isBuffTarget = false;
			block[y][x].isHealTarget = false;
			block[y][x].state = AstarBlockState::None; // A*表示のリセット
		}
	}

	// 行動優先順位のリセット
	enemy_.priorityReset();

	// 移動先優先度決定戦
	DecideAction(enemy_, player_); // enemyActionTarget が設定される

	// 優勝者の確認
	float maxPriority = enemy_.actPattern.GetHighestPriority();

	// 移動系（移動先の設定）
	if (maxPriority == enemy_.actPattern.MoveToAdvantage || maxPriority == enemy_.actPattern.MoveToEnemy)
	{
		// 決定された移動目標地点までパスを計算し、グローバルなpathNodesに格納
		Astar(enemy_.transforms.translate, IndexToPosition(enemyActionTarget));
	}

	
	// スキル系（スキル適応範囲のフラグ）
	else if (maxPriority == enemy_.actPattern.Skill || maxPriority == enemy_.actPattern.AdvancedSkill || maxPriority == enemy_.actPattern.UltimateSkill)
	{
		// スキルの種類に応じて範囲を計算し、ブロックをマーク
		int skillRange = 0;
		if (maxPriority == enemy_.actPattern.Skill)
		{
			skillRange = skillList[enemy_.skill].range;
		}
		else if (maxPriority == enemy_.actPattern.AdvancedSkill)
		{
			skillRange = advancedSkillList[enemy_.advancedSkill].range;
		}
		else if (maxPriority == enemy_.actPattern.UltimateSkill)
		{
			skillRange = ultimateSkillList[enemy_.ultimateSkill].range;
		}

		Vector2int enemyPos = PositionToIndex(enemy_.transforms.translate);
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				int dx = abs(x - enemyPos.x);
				int dy = abs(y - enemyPos.y);
				int distance = dx + dy; // マンハッタン距離

				if (distance <= skillRange)
				{
					block[y][x].isAttackTarget = true; // 攻撃対象としてマーク
				}
			}
		}
	}
}

void TR2Class::Update_EnemyConsiderTurn()
{
	turnRepuest = Turn::Enemy;
} 

void TR2Class::Draw_EnemyConsiderTurn()
{}


void TR2Class::Initialize_EnemyTurn()
{
	enemyMoveActCounter = 0; // そのターンの移動回数をリセット
	movement = false; // 移動フラグをリセット

	if (!pathNodes.empty())
	{
		movement = true;
		pathStepIndex = 0;
		astarStartIndex = PositionToIndex(enemy_.transforms.translate);
		astarTargetIndex = pathNodes[pathStepIndex];

		fromBlockTransforms = block[astarStartIndex.y][astarStartIndex.x].transforms;
		nextBlockTransforms = block[astarTargetIndex.y][astarTargetIndex.x].transforms;

		enemy_.pivot = { 0,0,0 }; // pivotをリセット
		if (astarStartIndex.x < astarTargetIndex.x)
		{
			direction = Direction::Right;
			enemy_.pivot.x = -BLOCK_WIDTH / 2.0f;
		}
		else if (astarStartIndex.x > astarTargetIndex.x)
		{
			direction = Direction::Left;
			enemy_.pivot.x = BLOCK_WIDTH / 2.0f;
		}
		else if (astarStartIndex.y < astarTargetIndex.y)
		{
			direction = Direction::Down;
			enemy_.pivot.z = -BLOCK_HEIGHT / 2.0f;
		}
		else if (astarStartIndex.y > astarTargetIndex.y)
		{
			direction = Direction::Up;
			enemy_.pivot.z = BLOCK_HEIGHT / 2.0f;
		}
	}
	// パスがない場合は、移動アニメーションは行われない (スキル使用など)
}

void TR2Class::Update_EnemyTurn()
{
	/*/////////////////////////////////////////////////////
		移動処理
	*//////////////////////////////////////////////////////
	if (movement && enemyMoveActCounter < enemy_.moveRange) // 移動可能範囲内である限り移動を続ける
	{
		if (translateBlock(CHARACTER_MOVE_EASING_TIME, enemy_.transforms))
		{
			// 1ステップの移動が完了
			enemyMoveActCounter++; // 移動回数をカウント
			astarStartIndex = astarTargetIndex;
			pathStepIndex++;

			if (pathStepIndex < pathNodes.size())
			{
				// 次の移動ステップを設定
				astarTargetIndex = pathNodes[pathStepIndex];
				fromBlockTransforms = block[astarStartIndex.y][astarStartIndex.x].transforms;
				nextBlockTransforms = block[astarTargetIndex.y][astarTargetIndex.x].transforms;

				// 移動方向に応じてpivotを調整
				enemy_.pivot = { 0,0,0 };
				if (astarStartIndex.x < astarTargetIndex.x)
				{
					direction = Direction::Right;
					enemy_.pivot.x = -BLOCK_WIDTH / 2.0f;
				}
				else if (astarStartIndex.x > astarTargetIndex.x)
				{
					direction = Direction::Left;
					enemy_.pivot.x = BLOCK_WIDTH / 2.0f;
				}
				else if (astarStartIndex.y < astarTargetIndex.y)
				{
					direction = Direction::Down;
					enemy_.pivot.z = -BLOCK_HEIGHT / 2.0f;
				}
				else if (astarStartIndex.y > astarTargetIndex.y)
				{
					direction = Direction::Up;
					enemy_.pivot.z = BLOCK_HEIGHT / 2.0f;
				}
			}
			else
			{
				// ゴール到達 (全ステップ完了) または移動可能範囲を使い切った
				movement = false;
				pathNodes.clear();
				enemy_.pivot = { 0,0,0 }; // 移動終了後のpivotリセット
			}
		}
	}
	else
	{
		// 移動が完了した、または移動しない場合
		// ここにスキルの使用ロジックなどを追加
		// 現状では移動後すぐにプレイヤーターンへ
		if (GetHitKey::keys[DIK_SPACE] && !movement) // デバッグ用: スペースキーで手動ターン終了
		{
			turnRepuest = Turn::Player;
		}
		else if (!movement) // 移動が終わったら自動でターン終了 (開発中は手動切り替えの方が便利かも)
		{
			turnRepuest = Turn::Player;
		}
	}
	/*/////////////////////////////////////////////////////
					スキルの使用
	*//////////////////////////////////////////////////////


	if (GetHitKey::keys[DIK_SPACE] && !movement)turnRepuest = Turn::Player;
	//if (!movement)turnRepuest = Turn::Player;
}

void TR2Class::Draw_EnemyTurn()
{
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (block[y][x].isAttackTarget)
			{
				Game::Drawobj({ block[y][x].transforms.scale,block[y][x].transforms.rotate,{block[y][x].transforms.translate.x,block[y][x].transforms.translate.y + 1,block[y][x].transforms.translate.z} }, { 0,0,0 }, playerModel, uvCheckerPng, 0xFFFFFFFF);
			}
		}
	}
}


void TR2Class::DecideAction(Charactor & self, const Charactor & enemy)
{
	int enemyToPlayerDist = GetShortestPathLength(self.transforms.translate, enemy.transforms.translate);
	int enemyToAdvantageDist = GetShortestPathLength(self.transforms.translate, Environment::高台);

	///////   アタッカー用の行動パターン


	///  ウルト  ///
	// 改善pt：より複数の敵にあてれるように移動、移動した後狙いの敵が移動したら意味ない
	if (
		self.delayCost < 0 && 										// ウルトたまってる
		skillList[self.skill].range >= enemyToPlayerDist			// ウルト射程圏内に敵がいる
		)
	{
		self.actPattern.UltimateSkill *= 10.0f;
	}
	else
	{
		self.actPattern.UltimateSkill *= 0.0f;
	}


	///  強スキル  ///
	// 改善pt：強スキルのデメリットは？スキルとの差分化要素は？使いどころは？
	if (advancedSkillList[self.advancedSkill].range >= enemyToPlayerDist)	// 強スキル射程圏内に敵がいる
	{
		self.actPattern.AdvancedSkill *= 9.0f;
	}
	else
	{
		self.actPattern.AdvancedSkill *= 0.0f;
	}

	///  スキル  ///
	if (skillList[self.skill].range >= enemyToPlayerDist)	// 強スキル射程圏内に敵がいる
	{
		self.actPattern.Skill *= 9.0f;
	}
	else
	{
		self.actPattern.Skill *= 0.0f;
	}

	///  有利ポジ移動  ///
	// 改善pt：敵から有利ポジの距離 > 自身から有利ポジの距離　なら悩め
	if (
		enemyToAdvantageDist < enemyToPlayerDist	// 有利ポジまでの距離　＜　敵までの距離
		)
	{
		self.actPattern.MoveToAdvantage *= 8.0f;
	}
	else
	{
		self.actPattern.MoveToAdvantage *= 0.0f;
	}

	/// 敵へ移動 ///
	// 有利ポジへの移動が選ばれず、敵が射程外の場合、敵へ近づく
	if (self.actPattern.MoveToAdvantage == 0.0f && enemyToPlayerDist > 0 && enemyToPlayerDist != 1000) // 有利ポジ移動が優先されず、敵が到達可能
	{
		self.actPattern.MoveToEnemy *= 1.8f; // 基本優先度
		SetEnemyActionTarget(enemy.transforms.translate); // 敵の現在地を目標に設定
	}
	else
	{
		self.actPattern.MoveToEnemy *= 0.0f;
	}


	// どの行動も選ばれなかった場合（全ての優先度が0の場合など）
	if (self.actPattern.GetHighestPriority() == 0.0f)
	{
		// 何もしない、またはデフォルトの行動を設定する
		SetEnemyActionTarget(self.transforms.translate); // その場に留まる
	}




}

bool TR2Class::translateBlock(float EasingMax, Transforms& transforms)
{
	movementT += 1.0f / 60.0f;

	//// S
	//block[fromIndex.y][fromIndex.x].transforms.scale.x = Easings::OUT_SINE(fromBlockTransforms.scale.x, nextBlockTransforms.scale.x, movementT / movementTmax);
	//block[fromIndex.y][fromIndex.x].transforms.scale.y = Easings::OUT_SINE(fromBlockTransforms.scale.y, nextBlockTransforms.scale.y, movementT / movementTmax);
	//block[fromIndex.y][fromIndex.x].transforms.scale.z = Easings::OUT_SINE(fromBlockTransforms.scale.z, nextBlockTransforms.scale.z, movementT / movementTmax);
	//block[nextIndex.y][nextIndex.x].transforms.scale.x = Easings::OUT_SINE(nextBlockTransforms.scale.x, fromBlockTransforms.scale.x, movementT / movementTmax);
	//block[nextIndex.y][nextIndex.x].transforms.scale.y = Easings::OUT_SINE(nextBlockTransforms.scale.y, fromBlockTransforms.scale.y, movementT / movementTmax);
	//block[nextIndex.y][nextIndex.x].transforms.scale.z = Easings::OUT_SINE(nextBlockTransforms.scale.z, fromBlockTransforms.scale.z, movementT / movementTmax);
	//// R
	//if (direction == Direction::Right)	block[nextIndex.y][nextIndex.x].transforms.rotate.z = Easings::OUT_SINE(fromBlockTransforms.rotate.z, fromBlockTransforms.rotate.z + 3.1415926f, movementT / movementTmax);
	//if (direction == Direction::Left)	block[nextIndex.y][nextIndex.x].transforms.rotate.z = Easings::OUT_SINE(fromBlockTransforms.rotate.z, fromBlockTransforms.rotate.z - 3.1415926f, movementT / movementTmax);
	//if (direction == Direction::Down)	block[nextIndex.y][nextIndex.x].transforms.rotate.x = Easings::OUT_SINE(fromBlockTransforms.rotate.x, fromBlockTransforms.rotate.x - 3.1415926f, movementT / movementTmax);
	//if (direction == Direction::Up)		block[nextIndex.y][nextIndex.x].transforms.rotate.x = Easings::OUT_SINE(fromBlockTransforms.rotate.x, fromBlockTransforms.rotate.x + 3.1415926f, movementT / movementTmax);
	//// T
	//block[fromIndex.y][fromIndex.x].transforms.translate.x = Easings::OUT_SINE(fromBlockTransforms.translate.x, nextBlockTransforms.translate.x, movementT / movementTmax);
	//block[fromIndex.y][fromIndex.x].transforms.translate.y = Easings::OUT_SINE(fromBlockTransforms.translate.y, nextBlockTransforms.translate.y, movementT / movementTmax);
	//block[fromIndex.y][fromIndex.x].transforms.translate.z = Easings::OUT_SINE(fromBlockTransforms.translate.z, nextBlockTransforms.translate.z, movementT / movementTmax);
	
	// player
	transforms.translate.x = Easings::OUT_SINE(fromBlockTransforms.translate.x, nextBlockTransforms.translate.x, movementT / EasingMax);
	transforms.translate.y = Easings::OUT_SINE(fromBlockTransforms.translate.y, nextBlockTransforms.translate.y, movementT / EasingMax);
	transforms.translate.z = Easings::OUT_SINE(fromBlockTransforms.translate.z, nextBlockTransforms.translate.z, movementT / EasingMax);


	if (movementT > EasingMax)
	{
		transforms.translate = nextBlockTransforms.translate;
		//for (int y = 0; y < MAP_HEIGHT; ++y)
		//{
		//	for (int x = 0; x < MAP_WIDTH; ++x)
		//	{
		//		block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
		//		block[y][x].transforms.translate = IndexToPosition({ x,y });
		//		block[y][x].transforms.rotate = { 0.0f,0.0f,0.0f };
		//		block[y][x].pivot = { 0,0,0 };
		//	}
		//}

		//Transforms box = block[nextIndex.y][nextIndex.x].transforms;
		//block[nextIndex.y][nextIndex.x].transforms = block[fromIndex.y][fromIndex.x].transforms;
		//block[fromIndex.y][fromIndex.x].transforms = box;

		//Block tmp = block[nextIndex.y][nextIndex.x];
		//block[nextIndex.y][nextIndex.x] = block[fromIndex.y][fromIndex.x];
		//block[fromIndex.y][fromIndex.x] = tmp;

		//for (int y = 0; y < MAP_HEIGHT; ++y)
		//{
		//	for (int x = 0; x < MAP_WIDTH; ++x)
		//	{
		//		block[y][x].pivot = { 0,0,0 };
		//	}
		//}


		movementT = 0;
		return true;
	}
	else
	{
		return false;
	}
}

Vector2int TR2Class::PositionToIndex(Vector3 pos)
{
	Vector2int index;
	// ブロック中心座標からインデックスを計算
	index.x = static_cast<int>(std::round(-pos.x / BLOCK_WIDTH));
	index.y = static_cast<int>(std::round(-pos.z / BLOCK_HEIGHT));
	// 範囲外の値を制限
	index.x = std::clamp(index.x, 0, MAP_WIDTH - 1);
	index.y = std::clamp(index.y, 0, MAP_HEIGHT - 1);
	return index;
}

Vector3 TR2Class::IndexToPosition(Vector2int index)
{
	Vector3 pos;

	pos.x = -static_cast<float>(index.x) * BLOCK_WIDTH;
	pos.y = 0.0f;
	pos.z = -static_cast<float>(index.y) * BLOCK_HEIGHT;

	return pos;
}

// fromIndex, toIndex間の移動がルール上可能か判定
bool TR2Class::CanMove(const Vector2int& from, const Vector2int& to) const
{
	if (to.x < 0 || to.x >= MAP_WIDTH || to.y < 0 || to.y >= MAP_HEIGHT) return false;
	BlockType fromType = block[from.y][from.x].type;
	BlockType toType = block[to.y][to.x].type;

	if (fromType == BlockType::Empty && toType == BlockType::Slope) return true;
	if (fromType == BlockType::Slope && toType == BlockType::Empty) return true;

	if (fromType == BlockType::Slope && toType == BlockType::Wall) return true;
	if (fromType == BlockType::Wall && toType == BlockType::Slope) return true;

	if (fromType == BlockType::Empty && toType == BlockType::Empty) return true;
	if (fromType == BlockType::Slope && toType == BlockType::Slope) return true;
	if (fromType == BlockType::Wall && toType == BlockType::Wall) return true;
	if (fromType == BlockType::WarpIn || toType == BlockType::WarpOut) return true;
	return false;
}

bool TR2Class::_RunAstar(
	const Vector2int& start,
	const Vector2int& target,
	std::map<Vector2int, Vector2int>& outParentMap,
	std::vector<std::vector<int>>& outGCostMap,
	std::vector<Vector2int>* outPathNodes
)
{
	// マップとリストの初期化
	for (auto& row : outGCostMap)
	{
		std::fill(row.begin(), row.end(), (std::numeric_limits<int>::max)());
	}
	outParentMap.clear();
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> currentOpenList; // ローカルなOpenList

	// スタート地点またはターゲット地点が不正なら終了
	if (start.x < 0 || start.x >= MAP_WIDTH || start.y < 0 || start.y >= MAP_HEIGHT ||
		target.x < 0 || target.x >= MAP_WIDTH || target.y < 0 || target.y >= MAP_HEIGHT ||
		block[start.y][start.x].type == BlockType::Wall) // スタートが壁なら移動不可
	{
		return false;
	}

	outGCostMap[start.y][start.x] = 0;
	int hCost = static_cast<int>(std::fabs(start.x - target.x) + std::fabs(start.y - target.y));
	currentOpenList.push({ start, 0, hCost, hCost, {-1, -1} }); // 親はなしで初期化

	while (!currentOpenList.empty())
	{
		Node current = currentOpenList.top();
		currentOpenList.pop();

		// すでに最適な経路で処理済みならスキップ (Fコストが同じでもGコストが低い方が優先)
		if (current.gCost > outGCostMap[current.index.y][current.index.x])
		{
			continue;
		}

		// 目的地に到達したらパスを再構築して返す
		if (current.index == target)
		{
			if (outPathNodes)
			{ // パスが必要な場合のみ再構築
				outPathNodes->clear();
				Vector2int pathStep = target;
				while (pathStep != start)
				{
					// ワープアウト地点の場合、ワープイン地点もパスに含める
					if (block[pathStep.y][pathStep.x].type == BlockType::WarpOut)
					{
						Vector2int warpInIndex = { -1, -1 };
						for (int y = 0; y < MAP_HEIGHT; ++y)
						{
							for (int x = 0; x < MAP_WIDTH; ++x)
							{
								if (block[y][x].type == BlockType::WarpIn)
								{
									warpInIndex = { x, y };
									break;
								}
							}
							if (warpInIndex.x != -1) break;
						}
						if (warpInIndex.x != -1)
						{
							outPathNodes->push_back(warpInIndex);
						}
					}

					outPathNodes->push_back(pathStep);
					auto it = outParentMap.find(pathStep);
					if (it == outParentMap.end() || (it->second.x == -1 && it->second.y == -1))
					{
						// 経路構築エラー、または親が見つからない
						outPathNodes->clear();
						return false;
					}
					pathStep = it->second; // 親ノードへ移動
				}
				std::reverse(outPathNodes->begin(), outPathNodes->end()); // パスを逆順にする
			}
			return true; // パスが見つかった
		}

		// 親を記録 (これがないとパス再構築ができない)
		// target以外でポップされたノードのみ親を記録
		outParentMap[current.index] = current.parentIndex;

		// 隣接ノードの探索 (上下左右)
		int dx[4] = { -1, 0, 1, 0 };
		int dy[4] = { 0, 1, 0, -1 };
		Direction neighborDirections[4] = { Direction::Left, Direction::Down, Direction::Right, Direction::Up };

		for (int i = 0; i < 4; ++i)
		{
			Vector2int neighborIndex = { current.index.x + dx[i], current.index.y + dy[i] };

			// 境界チェックと移動可否チェック
			if (!CanMove(current.index, neighborIndex))
			{
				continue;
			}

			// 新しいGコストの計算
			// 現在のノードのGコスト + 隣接ノードへの移動コスト (ここでは1) + 隣接ノードの固有コスト
			int newGCost = current.gCost + 1 + block[neighborIndex.y][neighborIndex.x].cost;

			// ワープ入り口の場合の特別な処理
			if (block[neighborIndex.y][neighborIndex.x].type == BlockType::WarpIn)
			{
				Vector2int warpOutIndex = { -1, -1 };
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						if (block[y][x].type == BlockType::WarpOut)
						{
							warpOutIndex = { x, y };
							break;
						}
					}
					if (warpOutIndex.x != -1) break;
				}
				if (warpOutIndex.x != -1)
				{
					// ワープ出口への移動コストを加算
					// この場合は、ワープ出口を隣接ノードとみなしてコストを計算し、openListに入れるべき
					// ここでは簡略化のため、ワープ出口への移動コストを0と仮定し、新しいGコストを計算し直す
					int costToWarpOut = 1; // ワープインからワープアウトへの移動コスト
					newGCost = current.gCost + costToWarpOut; // current.gCostからワープ出口までのコスト
					// ワープ出口をそのまま次のneighborIndexとして扱う
					neighborIndex = warpOutIndex;
				}
				else
				{
					// ワープ出口がない場合、ワープ入り口は通常のEmptyブロックとして扱うか、移動不可にする
					// ここでは通常のコスト計算を続ける
				}
			}


			// より良い経路が見つかった場合
			if (newGCost < outGCostMap[neighborIndex.y][neighborIndex.x])
			{
				outGCostMap[neighborIndex.y][neighborIndex.x] = newGCost; // Gコストを更新
				// Hコスト（マンハッタン距離）
				int neighborHCost = static_cast<int>(std::fabs(neighborIndex.x - target.x) + std::fabs(neighborIndex.y - target.y));
				// Fコスト = Gコスト + Hコスト
				int neighborFCost = newGCost + neighborHCost;

				// OpenListにノードを追加
				currentOpenList.push({ neighborIndex, newGCost, neighborHCost, neighborFCost, current.index, neighborDirections[i], (block[neighborIndex.y][neighborIndex.x].type == BlockType::Slope) });
				// グローバルなA*実行の場合のみ、ブロックの状態をFrontierに設定
				if (&outGCostMap == &gCostMap) // グローバルなgCostMapが使われている場合
				{
					block[neighborIndex.y][neighborIndex.x].state = AstarBlockState::Frontier;
				}
			}
		}
	}
	return false; // パスが見つからなかった
}

void TR2Class::Astar(const Vector3& pos, const Vector3& target)
{
	astarStartIndex = PositionToIndex(pos);
	astarTargetIndex = PositionToIndex(target);

	// A*パス表示のためにブロックの状態をリセット
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].state = AstarBlockState::None;
		}
	}

	// グローバルなA*変数を使用してパスを計算し、pathNodesに結果を格納
	if (_RunAstar(astarStartIndex, astarTargetIndex, parentMap, gCostMap, &pathNodes))
	{
		aStarActive = true;
		// パス上のブロックをマーク
		for (const auto& nodeIndex : pathNodes)
		{
			block[nodeIndex.y][nodeIndex.x].state = AstarBlockState::Path;
		}
		// 開始地点もパスとしてマーク
		block[astarStartIndex.y][astarStartIndex.x].state = AstarBlockState::Path;
	}
	else
	{
		aStarActive = false;
		pathNodes.clear(); // パスが見つからなかった場合はクリア
	}
}

int TR2Class::GetShortestPathLength(const Vector3& pos, const Vector3& target)
{
	// ローカルなA*変数を使用してパス長のみを計算
	std::map<Vector2int, Vector2int> localParentMap;
	std::vector<std::vector<int>> localGCostMap(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, (std::numeric_limits<int>::max)()));
	std::vector<Vector2int> localPathNodes; // パス長計算用

	Vector2int localStartIndex = PositionToIndex(pos);
	Vector2int localTargetIndex = PositionToIndex(target);

	if (_RunAstar(localStartIndex, localTargetIndex, localParentMap, localGCostMap, &localPathNodes))
	{
		return static_cast<int>(localPathNodes.size());
	}
	return 1000; // パスが見つからなかった場合の大きな値（到達不能とみなす）
}
int TR2Class::GetShortestPathLength(const Vector3& pos, const Environment& targetType)
{
	int minPathLength = (std::numeric_limits<int>::max)(); // 無限大で初期化
	Vector2int currentPosIndex = PositionToIndex(pos);

	if (targetType == Environment::高台)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].type == BlockType::Wall) // 高台はWallブロックと定義
				{
					// 現在のキャラクター位置からこの高台までの距離を計算
					int pathLen = GetShortestPathLength(pos, IndexToPosition({ x, y }));
					if (pathLen < minPathLength)
					{
						minPathLength = pathLen;
					}
				}
			}
		}
	}
	// 他のEnvironmentタイプもここに追加可能

	return minPathLength; // 最短パス長を返す (見つからなければmax_int)
}

void TR2Class::SetEnemyActionTarget(const Vector3& target)
{
	enemyActionTarget = PositionToIndex(target);
}

void TR2Class::SetEnemyActionTarget(const Vector3& pos, const Environment& targetType)
{
	int minPathLength = (std::numeric_limits<int>::max)();
	Vector2int bestTargetIndex = PositionToIndex(pos); // 初期値は現在地

	if (targetType == Environment::高台)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].type == BlockType::Wall) // 高台はWallブロックと定義
				{
					int pathLen = GetShortestPathLength(pos, IndexToPosition({ x, y }));
					if (pathLen < minPathLength)
					{
						minPathLength = pathLen;
						bestTargetIndex = { x, y };
					}
				}
			}
		}
	}
	// もし有効なターゲットが見つかれば、それを設定。見つからなければ現在地のまま。
	if (minPathLength != (std::numeric_limits<int>::max)())
	{
		enemyActionTarget = bestTargetIndex;
	}
	else
	{
		// パスが見つからない場合は、現在の位置を目標とする（移動しない）
		enemyActionTarget = PositionToIndex(pos);
	}
}


void TR2Class::_UpdateMapEditMode()
{
	// ブロックAABBの更新とマウスとの衝突判定
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].AABB = Game::CreateAABB(block[y][x].transforms, block[y][x].model);
			block[y][x].mouseTarget = Game::IsCollisionMouseRayAABB(block[y][x].AABB, block[y][x].model);
		}
	}

	// ブロック変更ロジック
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			// マウスがブロック上にあり、左クリックが押された「瞬間」のみ処理
			if (block[y][x].mouseTarget && Game::IsPressMouse(0) && !PrePressMouse)
			{
				switch (selectBlock)
				{
				case 0: // Empty
					block[y][x].type = BlockType::Empty;
					block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
					block[y][x].transforms.translate.y = 0.0f;
					block[y][x].cost = 0;
					break;
				case 1: // Wall
					block[y][x].type = BlockType::Wall;
					block[y][x].transforms.scale = { 1.0f, 5.0f, 1.0f }; // 壁なので高さを高く
					block[y][x].transforms.translate.y = 1.0f;
					block[y][x].cost = 1; // 壁の移動コストは高く設定する
					break;
				case 2: // Slope
					block[y][x].type = BlockType::Slope;
					block[y][x].transforms.scale = { 1.0f, 2.5f, 1.0f }; // 傾斜なので中間の高さ
					block[y][x].transforms.translate.y = 0.5f;
					block[y][x].cost = 0; // スロープの移動コストは低く
					break;
				case 3: // WarpIn
					// 既存のWarpInをEmptyに戻す（唯一性を保証）
					for (int wy = 0; wy < MAP_HEIGHT; ++wy)
					{
						for (int wx = 0; wx < MAP_WIDTH; ++wx)
						{
							if (block[wy][wx].type == BlockType::WarpIn)
							{
								block[wy][wx].type = BlockType::Empty;
								block[wy][wx].transforms.translate.y = 0.0f;
							}
						}
					}
					block[y][x].type = BlockType::WarpIn;
					block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
					block[y][x].transforms.translate.y = 0.0f;
					block[y][x].cost = 0;
					break;
				case 4: // WarpOut
					// 既存のWarpOutをEmptyに戻す（唯一性を保証）
					for (int wy = 0; wy < MAP_HEIGHT; ++wy)
					{
						for (int wx = 0; wx < MAP_WIDTH; ++wx)
						{
							if (block[wy][wx].type == BlockType::WarpOut)
							{
								block[wy][wx].type = BlockType::Empty;
								block[wy][wx].transforms.translate.y = 0.0f;
							}
						}
					}
					block[y][x].type = BlockType::WarpOut;
					block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
					block[y][x].transforms.translate.y = 0.0f;
					block[y][x].cost = 0;
					break;
				}
				block[y][x].changeFlag = true; // 変更フラグを設定（今回は単発クリックなので不要だが念のため）
			}
		}
	}

	// マウスボタンが離されたときにすべてのchangeFlagをリセット
	if (!Game::IsPressMouse(0) && PrePressMouse)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				block[y][x].changeFlag = false;
			}
		}
	}
	PrePressMouse = Game::IsPressMouse(0); // 次のフレームのために現在のマウス状態を保存
}
