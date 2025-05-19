#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s

#define TRIANGLE_SUM 60


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");
	//D3DResourceLeakChecker* debug;

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();



		///
		/// ↓更新処理ここから
		///


		///
		/// ↑更新処理ここまで
		///
		
		///
		/// ↓描画処理ここから
		///



		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();


	return 0;
}
