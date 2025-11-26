#include "Engine.h"
#include <numbers>
#include "GameManager/GameManager.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Engine::Instance().Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	Engine::Instance().BeginFrame();
	ResourceID::reload();
	GameManager* gm = new GameManager();
	Engine::Instance().EndFrame();

	while (Engine::Instance().ProcessMessage())
	{
		// フレームの開始
		Engine::Instance().BeginFrame();

		// ↓更新処理ここから
		gm->Update();

		// モデル情報の更新
		Engine::Instance().UpdateTransforms();

		// ↓描画処理ここから
		gm->Draw();

		// フレームの終了
		Engine::Instance().EndFrame();
	}

	delete gm;

	Engine::Instance().Finalize();

	return 0;
}