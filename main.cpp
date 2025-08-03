#include "Engine/Game.h"
#include <numbers>
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");

	GameManager gm = new GameManager();


	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		///
		/// ↓更新処理ここから
		///
		gm.Update();



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///




		if (ImGui::TreeNode("--------------Lighting--------------"))
		{
			static const char* modeName[] = { "Half Lambert", "Lambert","Nothig" };
			static int current_mode = 0;
			ImGui::Combo("LightingMode", &current_mode, modeName, IM_ARRAYSIZE(modeName));
			Game::ToggleLightMode(current_mode);

			ImGui::TreePop();
		}

		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}