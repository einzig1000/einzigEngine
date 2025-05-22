#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s

#define TRIANGLE_SUM 60


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");
	//D3DResourceLeakChecker* debug;

	int monsterBall = Game::LoadTexture("resources/monsterBall.png");

	Transforms transformSphere1;
	transformSphere1.scale = { 1.3f, 1.3f, 1.3f };
	transformSphere1.rotate = { 0.0f,4.72f,0.0f };

	// 球の分割数
	const uint32_t kSubdivision = 16;
	// 頂点リソースにデータを書き込む
	VertexData vertexData[kSubdivision * kSubdivision * 6];

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

		Game::DrawSphere(transformSphere1, vertexData, kSubdivision, monsterBall, { 1.0f, 1.0f, 1.0f, 1.0f });

		///
		/// ↑描画処理ここまで
		///
		
		if (ImGui::CollapsingHeader("sphere"))
		{
			ImGui::DragFloat3("SphereScale", &transformSphere1.scale.x, 0.01f);
			ImGui::DragFloat3("SphereRotate", &transformSphere1.rotate.x, 0.01f);
			ImGui::DragFloat3("SphereTranslate", &transformSphere1.translate.x, 0.01f);
		}



		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();


	return 0;
}
