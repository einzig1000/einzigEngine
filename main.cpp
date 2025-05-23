#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s

#define TRIANGLE_SUM 60


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");
	//D3DResourceLeakChecker* debug = nullptr;

	Transforms Transform;
	VertexData* vdata = nullptr;

	// 左下
	vdata[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vdata[0].texcoord = { 0.0f,1.0f };
	vdata[0].normal = { 0.0f,0.0f,-1.0f };
	// 左上
	vdata[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vdata[1].texcoord = { 0.0f,0.0f };
	vdata[1].normal = { 0.0f,0.0f,-1.0f };
	// 右下
	vdata[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vdata[2].texcoord = { 1.0f,1.0f };
	vdata[2].normal = { 0.0f,0.0f,-1.0f };
	// 右上
	vdata[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vdata[3].texcoord = { 1.0f,0.0f };
	vdata[3].normal = { 0.0f,0.0f,-1.0f };

	Vector4 color = { 1,0,0,1 };
	int i = Game::LoadTexture("resources/uvChecker.png");


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

		Game::DrawSprite(Transform, vdata, i, color);

		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();


	return 0;
}
