#include "TestPhase.h"
#include "engine/Camera/CameraController.h"

TestPhase::TestPhase()
{
	uint32_t playerTex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	uint32_t playerModel = Game::LoadModel("resources/Prototypes/model/", "cube.obj");

	uint32_t blockTex = Game::LoadTexture("resources/Prototypes/texture/white1x1.png");
	uint32_t blockModel = Game::LoadModel("resources/Prototypes/model/", "cube.obj");

	uint32_t enemyTex = Game::LoadTexture("resources/Prototypes/texture/white1x1.png");
	uint32_t enemyModel = Game::LoadModel("resources/Prototypes/model/", "corn.obj");

	model_.model = playerModel;
	model_.texture = playerTex;

	particle_.mono.model = enemyModel;
	particle_.mono.texture = enemyTex;
}

TestPhase::~TestPhase()
{
}

void TestPhase::Initialize()
{}


void TestPhase::Update()
{
}


void TestPhase::Draw()
{
	model_.Draw();
	particle_.mono.Draw();
	particle_.mono.DrawImGui();
	particle_.Draw();
	particle_.DrawEmitter();
}