#include "TestPhase.h"
#include "engine/Camera/CameraController.h"

TestPhase::TestPhase()
{
	uint32_t playerTex = Game::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	uint32_t playerModel = Game::LoadModel("resources/Prototypes/model/", "cube.obj");

	uint32_t enemyTex = Game::LoadTexture("resources/Prototypes/texture/circle.png");
	uint32_t enemyModel = Game::LoadModel("resources/Prototypes/model/", "plane.obj");

	model_.model = playerModel;
	model_.texture = playerTex;


	particle_.mono.model = enemyModel;
	// パーティクルテクスチャ
	particle_.mono.texture = enemyTex;
	// パーティクル描画オプション
	particle_.mono.options.blendMode = BlendMode::kBlendModeAdd;
	// パーティクル初期回転量
	particle_.mono.transforms.rotate = { 0.0f,0.0f,0.0f };
	// パーティクル初期拡縮量
	particle_.mono.transforms.scale = { 0.1f,0.1f,0.1f };
	// パーティクル初期カラー
	particle_.mono.color = 0xFFFFFFFF;

	// パーティクル毎フレーム加算される回転量
	particle_.AddRotate = { 0.2f,0.2f,0.2f };
	// パーティクル毎フレーム加算される拡縮量
	particle_.AddScale = { -0.005f,-0.005f,-0.005f };
	// パーティクル飛んでく向き
	particle_.target = { 10.0f, 0.0f, 0.0f };
	// パーティクルエミッター
	particle_.emitterAABB = { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } };
	particle_.emitterSphere = { { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } };
	// パーティクル速度
	particle_.velocity = 0.01f;
	// パーティクル生存時間
	particle_.liveMax = 1000;
	// パーティクル生まれるスパン/frame
	particle_.emissionDelay = 1;
	// パーティクルエミッター形状
	particle_.option.emitterShape = true;
}

TestPhase::~TestPhase()
{
}

void TestPhase::Initialize()
{}


void TestPhase::Update()
{
	particle_.mono.LookAtCamera(roll_);
	roll_ += 0.02f;
}


void TestPhase::Draw()
{
	//model_.Draw();
	//particle_.mono.Draw();
	//particle_.mono.DrawImGui();


	particle_.DrawImGui();
	particle_.Draw();
	particle_.DrawEmitter();
}