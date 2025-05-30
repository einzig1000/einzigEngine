#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s
#include "Easings.h"
#include "CG2_05.h"




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"CG2");
	CG2_05* cg = new CG2_05;


	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		///
		/// ↓更新処理ここから
		///
		cg->Update();
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		cg->Draw();

		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}

	delete cg;
	cg = nullptr;

	Game::Finalize();


	return 0;
}