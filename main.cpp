#include "Engine/Game.h"
#include <numbers>



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	int mod = Game::LoadOBJ("resources/block/", "map.obj");
	int tex = Game::LoadTexture("resources/block/map.png");

	Game::RenderData_Model test1;
	test1.model = mod;
	test1.texture = tex;
	test1.color = 0xFF0000FF;

	Game::RenderData_Model test2;
	test2.model = mod;
	test2.texture = tex;

	Game::RenderData_Model test3;
	test3.transforms.parentWorld = &test2.transforms.World;
	test3.model = mod;
	test3.texture = tex;
	test3.color = 0xFF0000FF;

	Vector3 target;

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		
		// ↓更新処理ここから
		
		test1.LookAtOnce(&test3);
		test3.LookAtOnce(&test1);
		//test1.LookAtOnce(test2.transforms.translate);		 


		if (GetHitKey::keys[DIK_1])
		{
			test1.LookAtCamera();
		}

		if (test2.isCollisionMouseRay)
		{
			test2.color = 0xFF00FFFF;
		}
		else
		{
			test2.color = 0xFFFFFFFF;
		}

		// ↓描画処理ここから
		test1.Draw();
		test2.Draw();
		test3.Draw();

		ImGui::DragFloat3("test1t", &test1.transforms.translate.x, 0.01f);
		ImGui::DragFloat3("test2t", &test2.transforms.translate.x, 0.01f);
		ImGui::DragFloat3("test3t", &test3.transforms.translate.x, 0.01f);
		ImGui::DragFloat3("test1r", &test1.transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("test2r", &test2.transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("test3r", &test3.transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("target", &target.x, 0.01f);

		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();

	return 0;
}