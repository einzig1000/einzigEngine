#include "Engine/Game.h"
#include <numbers>
#include "GameManager/GameManager.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	Game::BeginFrame();                      // コマンドリストをResetしてopenにする
	GameManager* gm = new GameManager();     // ここでテクスチャ読み込み（Uploadが記録される）
	Game::EndFrame();                        // Close→Execute→Present→Wait で反映

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();

		// ↓更新処理ここから
		gm->Update();

		// モデル情報の更新
		Game::UpdateTransforms();

		// ↓描画処理ここから
		gm->Draw();

		// フレームの終了
		Game::EndFrame();
	}

	delete gm;

	Game::Finalize();

	return 0;
}