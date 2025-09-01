#include "Engine/Game.h"
#include <numbers>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");


	// ここでスタックにオブジェクトを直接作成します
	Game::RenderData_Model test1;
	test1.model = Game::LoadOBJ("resources/block/", "map.obj");
	test1.texture = Game::LoadTexture("resources/block/map.png");

	Game::RenderData_Model test2;
	test2.model = Game::LoadOBJ("resources/block/", "map.obj");
	test2.texture = Game::LoadTexture("resources/block/map.png");

	Vector3 target;

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		
		// ↓更新処理ここから

		test1.Draw();
		test2.Draw();


		if (GetHitKey::keys[DIK_1])
		{
			test1.LookAtModel(&test2);
		}
		if (GetHitKey::keys[DIK_2])
		{
			test1.LookAtVector3(target);
		}
		if (GetHitKey::keys[DIK_2])
		{
			test1.LookAtCamera();
		}

		if (test1.isCollisionMouseRay)
		{
			test1.color = 0xFF00FFFF;
		}
		else
		{
			test1.color = 0xFFFFFFFF;
		}

		// ↓描画処理ここから

		ImGui::DragFloat3("test1", &test1.transforms.translate.x, 0.01f);
		ImGui::DragFloat3("test2", &test2.transforms.translate.x, 0.01f);
		ImGui::DragFloat3("target", &target.x, 0.01f);

		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();

	return 0;
}