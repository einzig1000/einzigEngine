#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");

	// 変数宣言
	int uvChecker = Game::LoadTexture("resources/uvChecker.png");
	int obj1 = Game::LoadOBJ("resources", "axis.obj");
	Transforms transformOBJ1;
	Transforms transformOBJ2;
	transformOBJ1.translate = { 1.0f,1.0f,1.0f };


	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();



		///
		/// ↓更新処理ここから
		///
		transformOBJ1.rotate.x += 0.01f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Game::Drawobj(transformOBJ1, obj1, uvChecker);
		Game::Drawobj(transformOBJ2, obj1, uvChecker);

		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();

	return 0;
}
