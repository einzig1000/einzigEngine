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
	// プレイヤー
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
#pragma region
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
#pragma endregion

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
	skyDomeTransforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDomeTransforms.translate = { 0.0f,0.0f,0.0f };
	skyDomeTransforms.rotate = { 0.0f,0.0f,0.0f };
	skyDomeOptions.enableWireframeMode = false;

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

	skyDomeOptions.uvTransform.translate.y += 0.001f;
	skyDomeOptions.uvTransform.translate.x += 0.0001f;
}

void TR2Class::Draw()
{
	// 天球
	Game::Drawobj(skyDomeTransforms, { 0,0,0 }, skyDomeModel, skyDomePng, 0xFFFFFFFF, skyDomeOptions);

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

	ImGui::Text("---------------block---------------");
	std::string label = std::format("block[{}][{}].scale", nextIndex.y, nextIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[nextIndex.y][nextIndex.x].transforms.scale.x, 0.01f);
	label = std::format("block[{}][{}].rotate", nextIndex.y, nextIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[nextIndex.y][nextIndex.x].transforms.rotate.x, 0.01f);
	label = std::format("block[{}][{}].translate", nextIndex.y, nextIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[nextIndex.y][nextIndex.x].transforms.translate.x, 0.01f);
	label = std::format("block[{}][{}].pivot", nextIndex.y, nextIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[nextIndex.y][nextIndex.x].pivot.x, 0.01f);


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

	PrePressMouse = Game::IsPressMouse(0);
}

void TR2Class::Initialize_PlayerTurn()
{
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].isAttackTarget = false;
			block[y][x].isBuffTarget = false;
			block[y][x].isHealTarget = false;
		}
	}


	moveFrom = PositionToIndex(player_.transforms.translate);
}

void TR2Class::Update_PlayerTurn()
{
	Vector2int candidate = moveFrom;

	if (GetHitKey::keys[DIK_S] && !GetHitKey::preKeys[DIK_S] && !movement)
	{
		candidate.y += 1;
		if (CanMove(moveFrom, candidate) && GetShortestPathLength(player_.transforms.translate, IndexToPosition(candidate)) <= player_.moveRange)
		{
			moveFrom = candidate;
		}
	}
	if (GetHitKey::keys[DIK_W] && !GetHitKey::preKeys[DIK_W] && !movement)
	{
		candidate = moveFrom;
		candidate.y -= 1;
		if (CanMove(moveFrom, candidate) && GetShortestPathLength(player_.transforms.translate, IndexToPosition(candidate)) <= player_.moveRange)
		{
			moveFrom = candidate;
		}
	}
	if (GetHitKey::keys[DIK_A] && !GetHitKey::preKeys[DIK_A] && !movement)
	{
		candidate = moveFrom;
		candidate.x += 1;
		if (CanMove(moveFrom, candidate) && GetShortestPathLength(player_.transforms.translate, IndexToPosition(candidate)) <= player_.moveRange)
		{
			moveFrom = candidate;
		}
	}
	if (GetHitKey::keys[DIK_D] && !GetHitKey::preKeys[DIK_D] && !movement)
	{
		candidate = moveFrom;
		candidate.x -= 1;
		if (CanMove(moveFrom, candidate) && GetShortestPathLength(player_.transforms.translate, IndexToPosition(candidate)) <= player_.moveRange)
		{
			moveFrom = candidate;
		}
	}
	if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE] && !movement)
	{
		Astar(player_.transforms.translate, IndexToPosition(moveFrom));
		startIndex = PositionToIndex(player_.transforms.translate);
		targetIndex = moveFrom;
		pathNodes.clear();

		if (!parentMap.empty() && startIndex != targetIndex)
		{ // パスが見つかり、スタートとゴールが異なる場合
			Vector2int tempPathStep = targetIndex;
			while (tempPathStep != startIndex)
			{
				auto it = parentMap.find(tempPathStep);
				if (it == parentMap.end() || (it->second.x == -1 && it->second.y == -1))
				{
					// 経路が見つからない場合
					pathNodes.clear();
					break;
				}
				pathNodes.push_back(tempPathStep);
				tempPathStep = it->second;
			}
		}
		std::reverse(pathNodes.begin(), pathNodes.end());
		if (!pathNodes.empty())
		{
			movement = true;
			pathStepIndex = 0;
			fromIndex = PositionToIndex(player_.transforms.translate);
			nextIndex = pathNodes[pathStepIndex];

			fromBlockTransforms = block[fromIndex.y][fromIndex.x].transforms;
			nextBlockTransforms = block[nextIndex.y][nextIndex.x].transforms;

			if (fromIndex.x < nextIndex.x)
			{
				direction = Direction::Right;
				block[nextIndex.y][nextIndex.x].pivot.x += BLOCK_WIDTH / 2.0f;
			}
			else if (fromIndex.x > nextIndex.x)
			{
				direction = Direction::Left;
				block[nextIndex.y][nextIndex.x].pivot.x -= BLOCK_WIDTH / 2.0f;
			}
			else if (fromIndex.y < nextIndex.y)
			{
				direction = Direction::Down;
				block[nextIndex.y][nextIndex.x].pivot.z += BLOCK_WIDTH / 2.0f;
			}
			else if (fromIndex.y > nextIndex.y)
			{
				direction = Direction::Up;
				block[nextIndex.y][nextIndex.x].pivot.z -= BLOCK_WIDTH / 2.0f;
			}
		}
	}

	if (movement)
	{
		if (translateBlock(0.5f, player_.transforms))
		{
			fromIndex = nextIndex;
			pathStepIndex++;
			if (pathStepIndex < pathNodes.size())
			{
				nextIndex = pathNodes[pathStepIndex];
				fromBlockTransforms = block[fromIndex.y][fromIndex.x].transforms;
				nextBlockTransforms = block[nextIndex.y][nextIndex.x].transforms;
				block[nextIndex.y][nextIndex.x].pivot = { 0,0,0 };

				if (fromIndex.x < nextIndex.x)
				{
					direction = Direction::Right;
					block[nextIndex.y][nextIndex.x].pivot.x += BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.x > nextIndex.x)
				{
					direction = Direction::Left;
					block[nextIndex.y][nextIndex.x].pivot.x -= BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.y < nextIndex.y)
				{
					direction = Direction::Down;
					block[nextIndex.y][nextIndex.x].pivot.z += BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.y > nextIndex.y)
				{
					direction = Direction::Up;
					block[nextIndex.y][nextIndex.x].pivot.z -= BLOCK_WIDTH / 2.0f;
				}
			}
			else
			{
				// ゴール到達
				movement = false;
				pathNodes.clear();
				turnRepuest = Turn::EnemyConsider;
			}
		}
	}
}

void TR2Class::Draw_PlayerTurn()
{
	Game::Drawobj(block[moveFrom.y][moveFrom.x].transforms, player_.pivot, player_.model, player_.texture, 0xFFFFFF55, player_.options);
	ImGui::Text("[moveFrom.x = %d][moveFrom.y = %d]", moveFrom.x, moveFrom.y);
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
		}
	}

	// 行動優先順位のリセット
	enemy_.priorityReset();
	// 移動開始地点の記録
	moveFrom = PositionToIndex(enemy_.transforms.translate);

	// 移動先優先度決定戦
	DecideAction(enemy_, player_);

	// 優勝者の確認
	float maxPriority = enemy_.actPattern.GetHighestPriority();

	// 移動系（移動先の設定）
	if (maxPriority == enemy_.actPattern.MoveToAdvantage || maxPriority == enemy_.actPattern.MoveToEnemy)
	{
		if (maxPriority == enemy_.actPattern.MoveToAdvantage)
		{
			// moveFromを最短有利ポジに
 			GetShortestPosition(IndexToPosition(moveFrom), Environment::高台);
		}
		else if (maxPriority == enemy_.actPattern.MoveToEnemy)
		{
			// moveFromを敵ポジに
			GetShortestPosition(IndexToPosition(moveFrom), player_.transforms.translate);
		}

		Astar(enemy_.transforms.translate, IndexToPosition(moveFrom));
	}
	
	// スキル系（スキル適応範囲のフラグ）
	else if (maxPriority == enemy_.actPattern.Skill || maxPriority == enemy_.actPattern.AdvancedSkill || maxPriority == enemy_.actPattern.UltimateSkill)
	{
		if (maxPriority == enemy_.actPattern.Skill)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					int dx = abs(x - moveFrom.x);
					int dy = abs(y - moveFrom.y);
					int distance = dx + dy;

					if (distance <= skillList[enemy_.skill].range)
					{
						block[y][x].isAttackTarget = true;
					}
				}
			}
		}
		else if (maxPriority == enemy_.actPattern.AdvancedSkill)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					int dx = abs(x - moveFrom.x);
					int dy = abs(y - moveFrom.y);
					int distance = dx + dy;

					if (distance <= advancedSkillList[enemy_.advancedSkill].range)
					{
						block[y][x].isAttackTarget = true;
					}
				}
			}
		}
		else if (maxPriority == enemy_.actPattern.UltimateSkill)
		{
			for (int y = 0; y < MAP_HEIGHT; ++y)
			{
				for (int x = 0; x < MAP_WIDTH; ++x)
				{
					int dx = abs(x - moveFrom.x);
					int dy = abs(y - moveFrom.y);
					int distance = dx + dy;

					if (distance <= ultimateSkillList[enemy_.ultimateSkill].range)
					{
						block[y][x].isAttackTarget = true;
					}
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
	startIndex = PositionToIndex(enemy_.transforms.translate);
	targetIndex = moveFrom;
	Astar(enemy_.transforms.translate, IndexToPosition(moveFrom));
	pathNodes.clear();

	if (!parentMap.empty() && startIndex != targetIndex)
	{ // パスが見つかり、スタートとゴールが異なる場合
		Vector2int tempPathStep = targetIndex;
		while (tempPathStep != startIndex)
		{
			auto it = parentMap.find(tempPathStep);
			if (it == parentMap.end() || (it->second.x == -1 && it->second.y == -1))
			{
				// 経路が見つからない場合
				pathNodes.clear();
				break;
			}
			pathNodes.push_back(tempPathStep);
			tempPathStep = it->second;
		}
	}
	std::reverse(pathNodes.begin(), pathNodes.end());
	if (!pathNodes.empty())
	{
		movement = true;
		pathStepIndex = 0;
		fromIndex = PositionToIndex(enemy_.transforms.translate);
		nextIndex = pathNodes[pathStepIndex];

		fromBlockTransforms = block[fromIndex.y][fromIndex.x].transforms;
		nextBlockTransforms = block[nextIndex.y][nextIndex.x].transforms;

		if (fromIndex.x < nextIndex.x)
		{
			direction = Direction::Right;
			block[nextIndex.y][nextIndex.x].pivot.x += BLOCK_WIDTH / 2.0f;
		}
		else if (fromIndex.x > nextIndex.x)
		{
			direction = Direction::Left;
			block[nextIndex.y][nextIndex.x].pivot.x -= BLOCK_WIDTH / 2.0f;
		}
		else if (fromIndex.y < nextIndex.y)
		{
			direction = Direction::Down;
			block[nextIndex.y][nextIndex.x].pivot.z += BLOCK_WIDTH / 2.0f;
		}
		else if (fromIndex.y > nextIndex.y)
		{
			direction = Direction::Up;
			block[nextIndex.y][nextIndex.x].pivot.z -= BLOCK_WIDTH / 2.0f;
		}
	}

}

void TR2Class::Update_EnemyTurn()
{
	/*/////////////////////////////////////////////////////
		移動処理(移動しない時も０マスの移動と表す)
	*//////////////////////////////////////////////////////
	if (movement && enemyMoveActCounter <= enemy_.moveRange)
	{
		if (translateBlock(0.5f, enemy_.transforms))
		{
			enemyMoveActCounter++;
			fromIndex = nextIndex;
			pathStepIndex++;
			if (pathStepIndex < pathNodes.size())
			{
				nextIndex = pathNodes[pathStepIndex];
				fromBlockTransforms = block[fromIndex.y][fromIndex.x].transforms;
				nextBlockTransforms = block[nextIndex.y][nextIndex.x].transforms;

				if (fromIndex.x < nextIndex.x)
				{
					direction = Direction::Right;
					block[nextIndex.y][nextIndex.x].pivot.x += BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.x > nextIndex.x)
				{
					direction = Direction::Left;
					block[nextIndex.y][nextIndex.x].pivot.x -= BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.y < nextIndex.y)
				{
					direction = Direction::Down;
					block[nextIndex.y][nextIndex.x].pivot.z += BLOCK_WIDTH / 2.0f;
				}
				else if (fromIndex.y > nextIndex.y)
				{
					direction = Direction::Up;
					block[nextIndex.y][nextIndex.x].pivot.z -= BLOCK_WIDTH / 2.0f;
				}
			}
			else
			{
				// ゴール到達
				movement = false;
				enemyMoveActCounter = 0;
				pathNodes.clear();
			}
		}
	}
	else
	{
		// ゴール到達
		movement = false;
		enemyMoveActCounter = 0;
		pathNodes.clear();
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

	///  スキル  ///
	if (skillList[self.skill].range >= enemyToPlayerDist)	// 強スキル射程圏内に敵がいる
	{
		self.actPattern.Skill *= 9.0f;
	}

	///  有利ポジ移動  ///
	// 改善pt：敵から有利ポジの距離 > 自身から有利ポジの距離　なら悩め
	if (
		enemyToAdvantageDist < enemyToPlayerDist	// 有利ポジまでの距離　＜　敵までの距離
		)
	{
		self.actPattern.MoveToAdvantage *= 8.0f;
	}







}

bool TR2Class::translateBlock(float EasingMax, Transforms& transforms)
{
	movementTmax = EasingMax;
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
	transforms.translate.x = Easings::OUT_SINE(fromBlockTransforms.translate.x, nextBlockTransforms.translate.x, movementT / movementTmax);
	transforms.translate.y = Easings::OUT_SINE(fromBlockTransforms.translate.y, nextBlockTransforms.translate.y, movementT / movementTmax);
	transforms.translate.z = Easings::OUT_SINE(fromBlockTransforms.translate.z, nextBlockTransforms.translate.z, movementT / movementTmax);


	movementT += 1.0f / 60.0f;
	if (movementT > movementTmax)
	{
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
		direction = Direction::None;
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


void TR2Class::AstarSet(const Vector3& pos, const Vector3& target)
{
	// ブロックの状態をリセット
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			block[y][x].state = AstarBlockState::None;
		}
	}
	// gCostMapを最大値で初期化
	for (auto& row : gCostMap)
	{
		std::fill(row.begin(), row.end(), (std::numeric_limits<int>::max)());
	}
	// openListの初期化
	openList = std::priority_queue<Node, std::vector<Node>, std::greater<Node>>();
	// openListの初期化
	while (!openList.empty()) // なんかclear()は危険らしい。知らんけど
	{
		openList.pop();
	}
	// parentMapの初期化
	parentMap.clear();

	// スタート地点とゴール地点を設定
	startIndex = PositionToIndex(pos);
	targetIndex = PositionToIndex(target);

	// スタートまたはターゲットが壁、またはマップ範囲外ならA*をアクティブにしない
	if (startIndex.x < 0 || startIndex.x >= MAP_WIDTH || startIndex.y < 0 || startIndex.y >= MAP_HEIGHT ||
		targetIndex.x < 0 || targetIndex.x >= MAP_WIDTH || targetIndex.y < 0 || targetIndex.y >= MAP_HEIGHT 
		//|| block[startIndex.y][startIndex.x].type == BlockType::Wall || block[targetIndex.y][targetIndex.x].type == BlockType::Wall
		)
	{
		aStarActive = false;
		return;
	}


	// 最短経路に間違いなく組み込まれるからスタート地点は探索済みとして扱う。
	block[startIndex.y][startIndex.x].state = AstarBlockState::Frontier;
	// スタート地点からスタート地点の距離
	gCostMap[startIndex.y][startIndex.x] = 0;
	// スタート地点からゴール地点の推定距離
	int hCost = static_cast<int>(std::fabs(startIndex.x - targetIndex.x) + std::fabs(startIndex.y - targetIndex.y));
	// 親はいないなので適当に{-1, -1} でも入れておく
	openList.push({ startIndex, 0, hCost, hCost, {-1, -1} });

	// 初期化完了
	aStarActive = true;
}

void TR2Class::Astar(const Vector3& pos, const Vector3& target)
{
	// 最初にAstarSetを呼び出して初期化
	if (!aStarActive) AstarSet(pos, target);

	// ワープ出口の座標を事前に探索
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

	while (!openList.empty())
	{
		Node current = openList.top();
		openList.pop();

		// すでにクローズリストにある（Explored）ノードはスキップ
		// OpenListに同じノードが複数入	っていた場合の対策、
		// より良い経路で既に処理済みの場合に古いノードを無視するために必要
		if (block[current.index.y][current.index.x].state == AstarBlockState::Explored)
		{
			continue;
		}

		// 現在のノードをクローズリストに追加
		block[current.index.y][current.index.x].state = AstarBlockState::Explored;
		parentMap[current.index] = current.parentIndex; // 親を記録

		// 目的地に到達したらループを終了
		if (current.index == targetIndex)
		{
			// 経路の再構築
			Vector2int pathStep = targetIndex;
			while (pathStep != startIndex)
			{
				block[pathStep.y][pathStep.x].state = AstarBlockState::Path;
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
					if (warpInIndex.x != -1) block[warpInIndex.y][warpInIndex.x].state = AstarBlockState::Path;
				}
				// 親ノードがマップに存在しない場合の安全対策
				auto it = parentMap.find(pathStep);
				if (it == parentMap.end() || (it->second.x == -1 && it->second.y == -1))
				{
					break;
				}
				pathStep = it->second; // 親ノードへ移動
			}
			block[startIndex.y][startIndex.x].state = AstarBlockState::Path; // スタート地点も経路としてマーク
			aStarActive = false; // 探索終了
			return; // 経路が見つかったので関数を終了
		}

		Direction dir[] = { Direction::Left, Direction::Down, Direction::Right, Direction::Up };
		int dx[4] = { -1, 0, 1, 0 };
		int dy[4] = { 0, 1, 0, -1 };

		// --- ここからA*探索の隣接ノード判定部 ---
		for (int i = 0; i < 4; ++i)
		{
			Vector2int neighborIndex = { current.index.x + dx[i], current.index.y + dy[i] };

			if (neighborIndex.x < 0 || neighborIndex.x >= MAP_WIDTH ||
				neighborIndex.y < 0 || neighborIndex.y >= MAP_HEIGHT)
			{
				continue;
			}

			BlockType fromType = block[current.index.y][current.index.x].type;
			BlockType toType = block[neighborIndex.y][neighborIndex.x].type;

			// --- ルール適用 ---
			if (!CanMove(current.index, neighborIndex))
			{
				continue;
			}

			// 既に探索済みはスキップ
			if (block[neighborIndex.y][neighborIndex.x].state == AstarBlockState::Explored)
			{
				continue;
			}

			// Slopeを通過したかどうか
			bool nextFromSlope = (toType == BlockType::Slope);

			int newGCost = current.gCost + block[neighborIndex.y][neighborIndex.x].cost + 1;

			if (newGCost < gCostMap[neighborIndex.y][neighborIndex.x])
			{
				gCostMap[neighborIndex.y][neighborIndex.x] = newGCost;
				int neighborHCost = static_cast<int>(std::fabs(neighborIndex.x - targetIndex.x) + std::fabs(neighborIndex.y - targetIndex.y));
				int neighborFCost = newGCost + neighborHCost + block[neighborIndex.y][neighborIndex.x].cost;
				openList.push({ neighborIndex, newGCost, neighborHCost, neighborFCost, current.index, dir[i], nextFromSlope });
				block[neighborIndex.y][neighborIndex.x].state = AstarBlockState::Frontier;
			}
		}

	}

	aStarActive = false; // 探索終了
}

int TR2Class::GetShortestPathLength(const Vector3& pos, const Vector3& target)
{
	std::vector<std::vector<int>> localGCostMap(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, (std::numeric_limits<int>::max)()));
	std::map<Vector2int, Vector2int> localParentMap;
	struct LocalNode
	{
		Vector2int index;
		int gCost;
		int hCost;
		int fCost;
		Vector2int parentIndex;
		bool fromSlope;
		bool operator>(const LocalNode& other) const
		{
			if (fCost != other.fCost) return fCost > other.fCost;
			if (gCost != other.gCost) return gCost > other.gCost;
			return false;
		}
	};
	std::priority_queue<LocalNode, std::vector<LocalNode>, std::greater<LocalNode>> localOpenList;

	Vector2int localStartIndex = PositionToIndex(pos);
	Vector2int localTargetIndex = PositionToIndex(target);

	if (localStartIndex.x < 0 || localStartIndex.x >= MAP_WIDTH || localStartIndex.y < 0 || localStartIndex.y >= MAP_HEIGHT ||
		localTargetIndex.x < 0 || localTargetIndex.x >= MAP_WIDTH || localTargetIndex.y < 0 || localTargetIndex.y >= MAP_HEIGHT)
	{
		return 1000;
	}

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

	localGCostMap[localStartIndex.y][localStartIndex.x] = 0;
	int hCost = static_cast<int>(std::fabs(localStartIndex.x - localTargetIndex.x) + std::fabs(localStartIndex.y - localTargetIndex.y));
	localOpenList.push({ localStartIndex, 0, hCost, hCost, {-1, -1}, false });

	while (!localOpenList.empty())
	{
		LocalNode current = localOpenList.top();
		localOpenList.pop();

		if (localParentMap.count(current.index)) continue;
		localParentMap[current.index] = current.parentIndex;

		if (current.index == localTargetIndex)
		{
			int pathLength = 0;
			Vector2int pathStep = localTargetIndex;
			while (pathStep != localStartIndex)
			{
				++pathLength;
				auto it = localParentMap.find(pathStep);
				if (it == localParentMap.end() || (it->second.x == -1 && it->second.y == -1))
				{
					return 1000;
				}
				pathStep = it->second;
			}
			return pathLength;
		}

		int dx[4] = { -1, 0, 1, 0 };
		int dy[4] = { 0, 1, 0, -1 };

		for (int i = 0; i < 4; ++i)
		{
			Vector2int neighborIndex = { current.index.x + dx[i], current.index.y + dy[i] };

			if (neighborIndex.x < 0 || neighborIndex.x >= MAP_WIDTH ||
				neighborIndex.y < 0 || neighborIndex.y >= MAP_HEIGHT)
			{
				continue;
			}

			if (!CanMove(current.index, neighborIndex))
			{
				continue;
			}

			if (localParentMap.count(neighborIndex)) continue;

			bool nextFromSlope = (block[neighborIndex.y][neighborIndex.x].type == BlockType::Slope);

			int newGCost = current.gCost + block[neighborIndex.y][neighborIndex.x].cost + 1;

			if (newGCost < localGCostMap[neighborIndex.y][neighborIndex.x])
			{
				localGCostMap[neighborIndex.y][neighborIndex.x] = newGCost;
				int neighborHCost = static_cast<int>(std::fabs(neighborIndex.x - localTargetIndex.x) + std::fabs(neighborIndex.y - localTargetIndex.y));
				int neighborFCost = newGCost + neighborHCost + block[neighborIndex.y][neighborIndex.x].cost;
				localOpenList.push({ neighborIndex, newGCost, neighborHCost, neighborFCost, current.index, nextFromSlope });
			}
		}

	}

	return 1000;
}

void TR2Class::GetShortestPosition(const Vector3& pos, const Vector3& target)
{
	moveFrom = PositionToIndex(target);
}

int TR2Class::GetShortestPathLength(const Vector3& pos, const Environment& target)
{
	// 最短距離とその座標を初期化
	int minPathLength = 1234;
	Vector2int bestWall = PositionToIndex(pos);

	if (target == Environment::高台)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].type == BlockType::Wall)
				{
					int pathLen = GetShortestPathLength(IndexToPosition(bestWall), IndexToPosition({x, y}));
					if (pathLen < minPathLength)
					{
						minPathLength = pathLen;
						bestWall = { x, y };
					}
				}
			}
		}
	}

	return minPathLength;
}

void TR2Class::GetShortestPosition(const Vector3& pos, const Environment& target)
{
	// 最短距離とその座標を初期化
	int minPathLength = 1234;
	Vector2int bestWall = PositionToIndex(pos);

	if (target == Environment::高台)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (block[y][x].type == BlockType::Wall)
				{
					int pathLen = GetShortestPathLength(IndexToPosition(bestWall), IndexToPosition({ x, y }));
					if (pathLen < minPathLength)
					{
						minPathLength = pathLen;
						bestWall = { x, y };
					}
				}
			}
		}
	}
	if (minPathLength < 1234)
	{
		moveFrom = bestWall;
	}
}
