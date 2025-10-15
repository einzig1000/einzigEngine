#include "TestPhase.h"
#include "engine/Camera/CameraController.h"

TestPhase::TestPhase()
{

	frame = 0;
	uint32_t playerTex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	uint32_t playerModel = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");

	uint32_t blockTex = Game::LoadTexture("resources/Prototypes/texture/white1x1.png");
	uint32_t blockModel = Game::LoadOBJ("resources/Prototypes/model/", "cube.obj");

	uint32_t enemyTex = Game::LoadTexture("resources/Prototypes/texture/white1x1.png");
	uint32_t enemyModel = Game::LoadOBJ("resources/Prototypes/model/", "corn.obj");

	// プレイヤー生成
	player_ = new Player(playerTex, playerModel);

	// ブロック生成
	for (uint32_t x = 0; x < blockW; ++x)
	{
		for (uint32_t y = 0; y < blockY; ++y)
		{
			block_[x][y].model = blockTex;
			block_[x][y].texture = blockModel;
			block_[x][y].transforms.translate = { (float)x, -1.0f, (float)y };
			player_->SetBlock(block_[x][y]);
		}
	}

	// エネミー生成
	for (uint32_t i = 0; i < enemyN; ++i)
	{
		enemy_[i] = new Enemy(enemyTex, enemyModel);
	}

	// カメラ初期化
	Game::MoveCameraRotate(Vector3{ 0.0f,-std::numbers::pi_v<float> / 2.0f,0.0f }, 0, EaseType::IN_BACK);
}

TestPhase::~TestPhase()
{
	delete player_;
	player_ = nullptr;

	for (uint32_t i = 0; i < enemyN; ++i)
	{
		delete enemy_[i];
		enemy_[i] = nullptr;
	}
}

void TestPhase::Initialize()
{}


void TestPhase::Update()
{
	if (frame > 1)
	{
		player_->Update();

		for (uint32_t i = 0; i < enemyN; ++i)
		{
			enemy_[i]->Update(*player_);
		}
	}

	Vector3 Dpos = player_->data.GetWorldPosition();
	Vector3 Upos = player_->data.GetWorldPosition();
	Vector3 Mpos = player_->data.GetWorldPosition();

	Dpos.y = -1.0f;

	if (Mpos.y < 10.0f)
	{
		Mpos.y = 10.0f;
		Upos.y = 20.0f;
	}
	if (Mpos.y > 10.0f)
	{
		Upos.y = Mpos.y + 10.0f;
	}

	FrameTwoVerticalPoints(Upos, Dpos, 0.0f, 0, EaseType::IN_BACK);

	frame++;
}


void TestPhase::Draw()
{
	for (uint32_t x = 0; x < blockW; ++x)
	{
		for (uint32_t y = 0; y < blockY; ++y)
		{
			block_[x][y].Draw();
		}
	}

	for (uint32_t i = 0; i < enemyN; ++i)
	{
		enemy_[i]->Draw();
	}

	player_->Draw();

	Game::DrawLine(Vector3{ 10,10,10 }, Vector3{ 100,100,100 }, 0xFF0000FF);
}

void TestPhase::FrameTwoVerticalPoints(const Vector3& p1, const Vector3& p2,
	float padding = 0.0f,
	int spendFrame = 20,
	EaseType easing = EaseType::OUT_CUBIC)
{
	// 中点を回転中心に
	const Vector3 mid{
		(p1.x + p2.x) * 0.5f,
		(p1.y + p2.y) * 0.5f,
		(p1.z + p2.z) * 0.5f
	};

	// 垂直FOV（CameraController::Update と同じ値に合わせる）
	constexpr float fovY = 0.45f; // rad
	const float dy = std::fabs(p1.y - p2.y);
	const float halfSpan = dy * 0.5f + padding;

	// 垂直方向に収めるのに必要な距離（少しマージンを掛ける）
	float targetDistance = halfSpan / std::tan(fovY * 0.5f);
	targetDistance *= 1.02f; // 2% の余裕

	// 近遠クリップに配慮（Update内の near=0.1f, far=100.0f に合わせる）
	targetDistance = std::clamp(targetDistance, 0.11f, 99.99f);

	// 回転は変えず、中心と距離のみ変更（イージング適用）
	//const int frames = (spendFrame <= 0) ? 1 : spendFrame;
	Game::MoveCameraCenter(mid, spendFrame, easing);
	Game::MoveCameraDistance(targetDistance, spendFrame, easing);

}