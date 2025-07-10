#include "Game.h"
#include "TR2Class.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"CG2");

	TR2Class* TR2Class_ = new TR2Class();
	TR2Class_->Initialize();

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		// ↓更新処理ここから
		TR2Class_->Update();

		// ↓描画処理ここから
		TR2Class_->Draw();


		// フレームの終了
		Game::EndFrame();
	}
	delete TR2Class_;
	Game::Finalize();

	return 0;
}