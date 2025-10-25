#include "TestPhase.h"
#include "engine/Camera/CameraController.h"

TestPhase::TestPhase()
{
	uint32_t playerTex = Game::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	uint32_t playerModel = Game::LoadModel("resources/Prototypes/model/", "cube.obj");

	uint32_t enemyTex = Game::LoadTexture("resources/Prototypes/texture/circle.png");
	uint32_t enemyModel = Game::LoadModel("resources/Prototypes/model/", "plane.obj");

	uint32_t enemyTex2 = Game::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	uint32_t enemyTex3 = Game::LoadTexture("resources/Prototypes/texture/white1x1.png");

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

	particle_.model = playerModel;
	particle_.texture = playerTex;
	particle_.filePath = "resources/Prototypes/particle/aaa";
	//particle_.LoadJson();

	particle2_.model = enemyModel;
	particle2_.texture = enemyTex;
	particle2_.filePath = "resources/Prototypes/particle/bbb";
	//particle2_.LoadJson();

	particle3_.model = enemyModel;
	particle3_.texture = enemyTex;
	particle3_.filePath = "resources/Prototypes/particle/ccc";
	//particle3_.LoadJson();

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
	model_.DrawAABB();
	model_.DrawImGui();

	sprite_.Draw();
	sprite_.DrawImGui();

	triangle_.Draw();
	triangle_.DrawImGui();

	particle_.Draw();
	particle_.DrawImGui();
	particle_.DrawEmitter();
	//particle2_.Draw();
	//particle2_.DrawImGui();
	//particle2_.DrawEmitter();
	//particle3_.Draw();
	//particle3_.DrawImGui();
	//particle3_.DrawEmitter();

	line_.Draw();
	line_.DrawImGui();
	line2_.Draw();
	line2_.DrawImGui();
	line3_.Draw();
	line3_.DrawImGui();
}