#include "WindowManager.h"  // ウィンドウ管理
#include "DirectXManager.h" // DirectX管理
#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// COM の初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	// 例外ハンドラの設定
	SetUnhandledExceptionFilter(ExportDump);

	// ウィンドウ、DrectX初期化
	WindowManager windowManager(1280, 720, L"CG2");
	DirectXManager dxManager(windowManager.GetHwnd(), 1280, 720);
	Game game(windowManager, dxManager);


	// 変数宣言
	int a = game.LoadTexture("resources/uvChecker.png");
	int obj1 = game.LoadOBJ("resources", "axis.obj");
	Transforms transformOBJ1;


	while (game.ProcessMessage())
	{
		// フレームの開始
		game.BeginFrame();



		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		game.Drawobj(transformOBJ1, obj1, a, 0);

		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		game.EndFrame();
	}

	//game.Run();

	return 0;
}
