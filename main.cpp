#include "Engine/Game.h"
#include <numbers>
#include "GameManager.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	GameManager* gm = new GameManager();

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		
		// ↓更新処理ここから
		
		
		gm->Update();

		
		// ↓描画処理ここから


		gm->Draw();


		// フレームの終了
		Game::EndFrame();
	}

	delete gm;

	Game::Finalize();

	return 0;
}