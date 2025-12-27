#include "Engine.h"
#include "GameManager/GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

#ifdef _DEBUG
	D3DResourceLeakChecker checker;
#endif

	// ウィンドウ、DrectX初期化
	Engine::Instance().Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	GameManager* gm = new GameManager();

	while (Engine::Instance().ProcessMessage())
	{
		// フレームの開始
		Engine::Instance().BeginFrame();

		// ↓更新処理ここから
		gm->Update();

		// ↓描画処理ここから
		gm->Draw();

		// ↓ImGui描画処理ここから
		gm->DrawImGui();

		// フレームの終了
		Engine::Instance().EndFrame();
	}

	delete gm;

	Engine::Instance().Finalize();

	return 0;
}