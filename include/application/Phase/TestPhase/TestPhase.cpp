#include "TestPhase.h"
#include "engine/Camera/CameraController.h"

TestPhase::TestPhase()
{
	uint32_t playerTex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	uint32_t playerModel = Game::LoadModel("resources/Prototypes/model/", "cube.obj");

	uint32_t enemyTex = Game::LoadTexture("resources/Prototypes/texture/circle.png");
	uint32_t enemyModel = Game::LoadModel("resources/Prototypes/model/", "plane.obj");

	model_.model = playerModel;
	model_.texture = playerTex;
	model_.name = "player";

	sprite_.texture = playerTex;

	triangle_.texture = playerTex;

	line_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });

	line2_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line2_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line2_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line2_.lineType = LineType::BezierCurve;

	line3_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line3_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line3_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line3_.lineType = LineType::SplineCurve;

	particle_.mono.model = enemyModel;
	// パーティクルテクスチャ
	particle_.mono.texture = enemyTex;
	// パーティクル描画オプション
	particle_.mono.options.blendMode = BlendMode::kBlendModeAdd;
	// パーティクル初期カラー
	particle_.mono.color = 0xFFFFFFFF;

	particle2_.mono.model = enemyModel;
	// パーティクルテクスチャ
	particle2_.mono.texture = enemyTex;
	// パーティクル描画オプション
	particle2_.mono.options.blendMode = BlendMode::kBlendModeAdd;
	// パーティクル初期カラー
	particle2_.mono.color = 0xFFFFFFFF;

	//// パーティクル毎フレーム加算される回転量
	//particle_.AddRotate = { 0.2f,0.2f,0.2f };
	//// パーティクル毎フレーム加算される拡縮量
	//particle_.AddScale = { -0.005f,-0.005f,-0.005f };
	//// パーティクル飛んでく向き
	//particle_.target = { 10.0f, 0.0f, 0.0f };
	//// パーティクルエミッター
	//particle_.emitterAABB = { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } };
	//particle_.emitterSphere = { { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } };
	//// パーティクル速度
	//particle_.velocity = 0.01f;
	//// パーティクル生存時間
	//particle_.liveMax = 1000;
	//// パーティクル生まれるスパン/frame
	//particle_.emissionDelay = 1;
	//// パーティクルエミッター形状
	//particle_.option.emitterShape = true;
}

TestPhase::~TestPhase()
{
}

void TestPhase::Initialize()
{}


void TestPhase::Update()
{
	if (GetHitKey::IsPressedDown(DIK_1))
	{
		model_.texture = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	}
	else if	(GetHitKey::IsPressedDown(DIK_2))
	{
		model_.texture = Game::LoadTexture("resources/Prototypes/texture/circle.png");
	}
	


	//model_.LookAtCamera(roll_);
	roll_ += 0.02f;
}


void TestPhase::Draw()
{
	model_.Draw();
	model_.DrawAABB();
	model_.DrawImGui();

	sprite_.Draw();
	sprite_.DrawImGui();

	triangle_.Draw();
	triangle_.DrawImGui();

	particle_.Draw();
	particle_.DrawImGui();
	particle_.DrawEmitter();
	particle2_.Draw();
	particle2_.DrawImGui();
	particle2_.DrawEmitter();

	line_.Draw();
	line_.DrawImGui();
	line2_.Draw();
	line2_.DrawImGui();
	line3_.Draw();
	line3_.DrawImGui();
}