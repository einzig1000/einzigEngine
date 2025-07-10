#include "TR2Class.h"
#include <format>

TR2Class::TR2Class()
	:gCostMap(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, (std::numeric_limits<int>::max)()))
{

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

	// エネミー
	enemy_.model = playerModel;
	enemy_.texture = uvCheckerPng;
	enemy_.transforms.scale = { 0.5f, 0.5f, 0.5f };
	enemy_.transforms.translate = IndexToPosition({ 10,10 });
	enemy_.transforms.rotate = { 0.0f,0.0f,0.0f };
	enemy_.pivot = { 0,0,0 };
	enemy_.color = 0xFFFFFFFF;
	enemy_.moveRange = 4;


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
			else if (block[y][x].type == BlockType::Asid)
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

	// ブロックオーラ実験
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
	std::string label = std::format("block[{}][{}].scale", fromIndex.y, fromIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[fromIndex.y][fromIndex.x].transforms.scale.x, 0.01f);
	label = std::format("block[{}][{}].rotate", fromIndex.y, fromIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[fromIndex.y][fromIndex.x].transforms.rotate.x, 0.01f);
	label = std::format("block[{}][{}].translate", fromIndex.y, fromIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[fromIndex.y][fromIndex.x].transforms.translate.x, 0.01f);
	label = std::format("block[{}][{}].pivot", fromIndex.y, fromIndex.x);
	ImGui::DragFloat3(label.c_str(), &block[fromIndex.y][fromIndex.x].pivot.x, 0.01f);
}

void TR2Class::Initialize_PlayerTurn()
{
	moveFrom = PositionToIndex(player_.transforms.translate);
}

void TR2Class::Update_PlayerTurn()
{
	if ((GetHitKey::keys[DIK_S] && !GetHitKey::preKeys[DIK_S] && !movement) && GetShortestPathLength(IndexToPosition({ moveFrom.x ,moveFrom.y+1 }), player_.transforms.translate) <= player_.moveRange)
	{
		moveFrom.y += 1;
	}
	if ((GetHitKey::keys[DIK_W] && !GetHitKey::preKeys[DIK_W] && !movement) && GetShortestPathLength(IndexToPosition({ moveFrom.x ,moveFrom.y-1 }), player_.transforms.translate) <= player_.moveRange)
	{
		moveFrom.y -= 1;
	}
	if ((GetHitKey::keys[DIK_A] && !GetHitKey::preKeys[DIK_A] && !movement) && GetShortestPathLength(IndexToPosition({ moveFrom.x+1 ,moveFrom.y }), player_.transforms.translate) <= player_.moveRange)
	{
		moveFrom.x += 1;
	}
	if ((GetHitKey::keys[DIK_D] && !GetHitKey::preKeys[DIK_D] && !movement) && GetShortestPathLength(IndexToPosition({ moveFrom.x-1 ,moveFrom.y }), player_.transforms.translate) <= player_.moveRange)
	{
		moveFrom.x -= 1;
	}
	if (GetHitKey::keys[DIK_SPACE])
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
		if (!pathNodes.empty())
		{
			movement = true;
			pathStepIndex = 1;
			fromIndex = PositionToIndex(player_.transforms.translate);
			nextIndex = pathNodes[pathStepIndex];

			fromBlockTranslate = block[fromIndex.y][fromIndex.x].transforms.translate;
			nextBlockTranslate = block[nextIndex.y][nextIndex.x].transforms.translate;
			fromBlockRotate = block[fromIndex.y][fromIndex.x].transforms.rotate;

			if (fromIndex.x < nextIndex.x) direction = Direction::Right;
			else if (fromIndex.x > nextIndex.x) direction = Direction::Left;
			else if (fromIndex.y < nextIndex.y) direction = Direction::Down;
			else if (fromIndex.y > nextIndex.y) direction = Direction::Up;
		}
	}

	if (movement)
	{
		if (translateBlock(3.5f, player_.transforms))
		{
			fromIndex = nextIndex;
			pathStepIndex++;
			if (pathStepIndex < pathNodes.size())
			{
				nextIndex = pathNodes[pathStepIndex];
				fromBlockTranslate = block[fromIndex.y][fromIndex.x].transforms.translate;
				nextBlockTranslate = block[nextIndex.y][nextIndex.x].transforms.translate;
				fromBlockRotate = block[fromIndex.y][fromIndex.x].transforms.rotate;

				if (fromIndex.x < nextIndex.x) direction = Direction::Right;
				else if (fromIndex.x > nextIndex.x) direction = Direction::Left;
				else if (fromIndex.y < nextIndex.y) direction = Direction::Down;
				else if (fromIndex.y > nextIndex.y) direction = Direction::Up;
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
	Game::Drawobj({ player_.transforms.scale,player_.transforms.rotate,IndexToPosition(moveFrom) }, player_.pivot, player_.model, player_.texture, 0xFFFFFF55, player_.options);
}


void TR2Class::Initialize_EnemyConsiderTurn()
{
	Astar(enemy_.transforms.translate, player_.transforms.translate);
}

void TR2Class::Update_EnemyConsiderTurn()
{
	if (GetHitKey::keys[DIK_SPACE] && GetHitKey::preKeys[DIK_SPACE])
	{
		turnRepuest = Turn::Enemy;
	}
}

void TR2Class::Draw_EnemyConsiderTurn()
{}


void TR2Class::Initialize_EnemyTurn()
{
	fromIndex = PositionToIndex(enemy_.transforms.translate);
	nextIndex = fromIndex;
	nextIndex.x -= 3;
}

void TR2Class::Update_EnemyTurn()
{
	if (GetHitKey::keys[DIK_SPACE] && GetHitKey::preKeys[DIK_SPACE])
	{
		turnRepuest = Turn::Player;
	}
}

void TR2Class::Draw_EnemyTurn()
{}

bool TR2Class::translateBlock(float EasingMax, Transforms& transforms)
{
	movementTmax = EasingMax;
	block[fromIndex.y][fromIndex.x].transforms.translate.x = Easings::OUT_SINE(fromBlockTranslate.x, nextBlockTranslate.x, movementT / movementTmax);
	block[fromIndex.y][fromIndex.x].transforms.translate.y = Easings::OUT_SINE(fromBlockTranslate.y, nextBlockTranslate.y, movementT / movementTmax);
	block[fromIndex.y][fromIndex.x].transforms.translate.z = Easings::OUT_SINE(fromBlockTranslate.z, nextBlockTranslate.z, movementT / movementTmax);
	transforms.translate.x = Easings::OUT_SINE(fromBlockTranslate.x, nextBlockTranslate.x, movementT / movementTmax);
	transforms.translate.y = Easings::OUT_SINE(fromBlockTranslate.y, nextBlockTranslate.y, movementT / movementTmax);
	transforms.translate.z = Easings::OUT_SINE(fromBlockTranslate.z, nextBlockTranslate.z, movementT / movementTmax);

	if (direction == Direction::Right)	block[nextIndex.y][nextIndex.x].transforms.rotate.z = Easings::OUT_SINE(fromBlockRotate.z, fromBlockRotate.z - 3.1415926f, movementT / movementTmax);
	if (direction == Direction::Left)	block[nextIndex.y][nextIndex.x].transforms.rotate.z = Easings::OUT_SINE(fromBlockRotate.z, fromBlockRotate.z + 3.1415926f, movementT / movementTmax);
	if (direction == Direction::Down)	block[nextIndex.y][nextIndex.x].transforms.rotate.x = Easings::OUT_SINE(fromBlockRotate.x, fromBlockRotate.x - 3.1415926f, movementT / movementTmax);
	if (direction == Direction::Up)		block[nextIndex.y][nextIndex.x].transforms.rotate.x = Easings::OUT_SINE(fromBlockRotate.x, fromBlockRotate.x + 3.1415926f, movementT / movementTmax);

	movementT += 1.0f / 60.0f;
	if (movementT > movementTmax)
	{
		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
				block[y][x].transforms.translate = IndexToPosition({ x,y });
				block[y][x].transforms.rotate = { 0.0f,0.0f,0.0f };
				block[y][x].pivot = { 0,0,0 };
			}
		}
		movement = false;
		movementT = 0;
		direction = Direction::None;

		//Block box = block[nextIndex.y][nextIndex.x];
		//block[nextIndex.y][nextIndex.x] = block[fromIndex.y][fromIndex.x];
		//block[fromIndex.y][fromIndex.x] = box;

		//for (int y = 0; y < MAP_HEIGHT; ++y)
		//{
		//	for (int x = 0; x < MAP_WIDTH; ++x)
		//	{
		//		block[y][x].transforms.scale = { 1.0f, 1.0f, 1.0f };
		//		block[y][x].transforms.translate = IndexToPosition({ x,y });
		//		//block[y][x].transforms.rotate = { 0.0f,0.0f,0.0f };
		//		block[y][x].pivot = { 0,0,0 };
		//	}
		//}
		////movement = false;
		//movementT = 0;
		//direction = Direction::None;
		//return true;
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
		targetIndex.x < 0 || targetIndex.x >= MAP_WIDTH || targetIndex.y < 0 || targetIndex.y >= MAP_HEIGHT ||
		block[startIndex.y][startIndex.x].type == BlockType::Wall || block[targetIndex.y][targetIndex.x].type == BlockType::Wall)
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
	if (!aStarActive)AstarSet(pos, target);

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
					if (warpInIndex.x != -1)block[warpInIndex.y][warpInIndex.x].state = AstarBlockState::Path;
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

		Direction dir[] = { Direction::Left, Direction::Down, Direction::Right, Direction::Up }; // 対応する方向


		int dx[4] = { -1, 0, 1, 0 };
		int dy[4] = { 0, 1, 0, -1 };

		for (int i = 0; i < 4; ++i)
		{
			Vector2int neighborIndex = { current.index.x + dx[i], current.index.y + dy[i] };

			// マップの境界チェック
			if (neighborIndex.x < 0 || neighborIndex.x >= MAP_WIDTH ||
				neighborIndex.y < 0 || neighborIndex.y >= MAP_HEIGHT)
			{
				continue;
			}

			// 壁であるか、すでにクローズリストにある場合はスキップ
			// Explored はすでに処理済みなので、基本的に再評価しない
			if (block[neighborIndex.y][neighborIndex.x].type == BlockType::Wall ||
				block[neighborIndex.y][neighborIndex.x].state == AstarBlockState::Explored)
			{
				continue;
			}

			// --- ワープ処理 ---
			if (block[neighborIndex.y][neighborIndex.x].type == BlockType::WarpIn && warpOutIndex.x != -1)
			{
				// ワープ先が壁や探索済みでなければワープ
				if (block[warpOutIndex.y][warpOutIndex.x].state != AstarBlockState::Explored)
				{
					int newGCost = current.gCost + 1; // ワープは1コストでOK
					if (newGCost < gCostMap[warpOutIndex.y][warpOutIndex.x])
					{
						gCostMap[warpOutIndex.y][warpOutIndex.x] = newGCost;
						int neighborHCost = static_cast<int>(std::fabs(warpOutIndex.x - targetIndex.x) + std::fabs(warpOutIndex.y - targetIndex.y));
						int neighborFCost = newGCost + neighborHCost;
						openList.push({ warpOutIndex, newGCost, neighborHCost, neighborFCost, current.index, dir[i] });
						block[warpOutIndex.y][warpOutIndex.x].state = AstarBlockState::Frontier;
					}
				}
				continue; // WarpIn自体は通過しない
			}


			// 新しいgCostを計算（隣接ノードへの移動コストは1と仮定）
			int newGCost = current.gCost + block[neighborIndex.y][neighborIndex.x].cost + 1;

			// より良い経路が見つかった場合、またはまだオープンリストにない場合
			// gCostMapに記録されている値よりnewGCostが小さい場合、より良い経路が見つかった
			if (newGCost < gCostMap[neighborIndex.y][neighborIndex.x])
			{
				gCostMap[neighborIndex.y][neighborIndex.x] = newGCost; // gCostを更新
				int neighborHCost = static_cast<int>(std::fabs(neighborIndex.x - targetIndex.x) + std::fabs(neighborIndex.y - targetIndex.y));
				int neighborFCost = newGCost + neighborHCost + block[neighborIndex.y][neighborIndex.x].cost;

				// オープンリストにノードを追加（または更新）
				openList.push({ neighborIndex, newGCost, neighborHCost, neighborFCost, current.index, dir[i] });
				// 探索待ちとしてマーク
				block[neighborIndex.y][neighborIndex.x].state = AstarBlockState::Frontier;
			}
		}
	}

	aStarActive = false; // 探索終了
}

int TR2Class::GetShortestPathLength(const Vector3& pos, const Vector3& target)
{
	// ローカルA*用データ構造
	std::vector<std::vector<int>> localGCostMap(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, (std::numeric_limits<int>::max)()));
	std::map<Vector2int, Vector2int> localParentMap;
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> localOpenList;

	Vector2int localStartIndex = PositionToIndex(pos);
	Vector2int localTargetIndex = PositionToIndex(target);

	// 範囲・壁チェック
	if (localStartIndex.x < 0 || localStartIndex.x >= MAP_WIDTH || localStartIndex.y < 0 || localStartIndex.y >= MAP_HEIGHT ||
		localTargetIndex.x < 0 || localTargetIndex.x >= MAP_WIDTH || localTargetIndex.y < 0 || localTargetIndex.y >= MAP_HEIGHT ||
		block[localStartIndex.y][localStartIndex.x].type == BlockType::Wall ||
		block[localTargetIndex.y][localTargetIndex.x].type == BlockType::Wall)
	{
		return 1000;
	}

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

	// 初期化
	localGCostMap[localStartIndex.y][localStartIndex.x] = 0;
	int hCost = static_cast<int>(std::fabs(localStartIndex.x - localTargetIndex.x) + std::fabs(localStartIndex.y - localTargetIndex.y));
	localOpenList.push({ localStartIndex, 0, hCost, hCost, {-1, -1} });

	// 探索
	while (!localOpenList.empty())
	{
		Node current = localOpenList.top();
		localOpenList.pop();

		// 既に探索済みならスキップ
		if (localParentMap.count(current.index)) continue;

		localParentMap[current.index] = current.parentIndex;

		if (current.index == localTargetIndex)
		{
			// 経路の長さをカウント
			int pathLength = 0;
			Vector2int pathStep = localTargetIndex;
			while (pathStep != localStartIndex)
			{
				++pathLength;
				auto it = localParentMap.find(pathStep);
				if (it == localParentMap.end() || (it->second.x == -1 && it->second.y == -1))
				{
					return 1000; // 経路が不完全
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

			if (block[neighborIndex.y][neighborIndex.x].type == BlockType::Wall ||
				localParentMap.count(neighborIndex))
			{
				continue;
			}

			// ワープ処理
			if (block[neighborIndex.y][neighborIndex.x].type == BlockType::WarpIn && warpOutIndex.x != -1)
			{
				if (!localParentMap.count(warpOutIndex))
				{
					int newGCost = current.gCost + 1;
					if (newGCost < localGCostMap[warpOutIndex.y][warpOutIndex.x])
					{
						localGCostMap[warpOutIndex.y][warpOutIndex.x] = newGCost;
						int neighborHCost = static_cast<int>(std::fabs(warpOutIndex.x - localTargetIndex.x) + std::fabs(warpOutIndex.y - localTargetIndex.y));
						int neighborFCost = newGCost + neighborHCost;
						localOpenList.push({ warpOutIndex, newGCost, neighborHCost, neighborFCost, current.index });
					}
				}
				continue;
			}

			int newGCost = current.gCost + block[neighborIndex.y][neighborIndex.x].cost + 1;

			if (newGCost < localGCostMap[neighborIndex.y][neighborIndex.x])
			{
				localGCostMap[neighborIndex.y][neighborIndex.x] = newGCost;
				int neighborHCost = static_cast<int>(std::fabs(neighborIndex.x - localTargetIndex.x) + std::fabs(neighborIndex.y - localTargetIndex.y));
				int neighborFCost = newGCost + neighborHCost + block[neighborIndex.y][neighborIndex.x].cost;
				localOpenList.push({ neighborIndex, newGCost, neighborHCost, neighborFCost, current.index });
			}
		}
	}

	// 経路が見つからなかった場合
	return 1000;
}
