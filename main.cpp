#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s
#include "Easings.h"




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"CG2");


	// テクスチャ
	int uvCheckerPng;
	uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	int blockPng;
	blockPng = Game::LoadTexture("resources/map.png");
	// モデル
	int blockModel;
	blockModel = Game::LoadOBJ("resources/model", "map.obj");


	Transforms blockTransforms;
	blockTransforms.scale = { 1.2f, 1.2f, 1.2f };
	blockTransforms.translate = { 0,0,0 };
	blockTransforms.rotate = { 0.0f,0.0f,0.0f };

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		///
		/// ↓更新処理ここから
		///
		
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Game::Drawobj(blockTransforms, {0,0,0}, blockModel, uvCheckerPng, 0xFFFFFFFF);
		

		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}