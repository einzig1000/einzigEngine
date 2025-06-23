#include "Engine/Game.h"
#include "Utilities/functions.h"
#include "Utilities/Easings.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"CG2");


	//// オーディオデータ
	int alert = Game::LoadAudio("resources/sound/SE/alert.wav");
	int buzzer = Game::LoadAudio("resources/sound/SE/buzzer.mp3");


	//// テクスチャ
	int uvCheckerPng;
	uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	int blockPng;
	blockPng = Game::LoadTexture("resources/map.png");
	// モデル
	int blockModel;
	blockModel = Game::LoadOBJ("resources/model", "map.obj");

	// 現在のマスター音量
	float masterVolume = Game::GetMasterVolume();
	// 現在のそれぞれの音量
	float alertVolume = Game::GetVolume(alert);
	float buzzerVolume = Game::GetVolume(buzzer);

	bool alertLoop = false;
	bool buzzerLoop = false;

	Transforms blockTransforms;
	blockTransforms.scale = { 1.2f, 1.2f, 1.2f };
	blockTransforms.translate = { 0,0,0 };
	blockTransforms.rotate = { 0.0f,0.0f,0.0f };
	Vector3 blockPivot = { 0,0,0 };

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();
		Game::SetMouseRay();


		///
		/// ↓更新処理ここから
		///
	
		if (GetHitKey::keys[DIK_1] && !GetHitKey::preKeys[DIK_1])
		{
			if (Game::IsAudioPlaying(alert))Game::StopAudio(alert);
			Game::PlayAudio(alert, alertLoop);
		}
		if (GetHitKey::keys[DIK_2] && !GetHitKey::preKeys[DIK_2])
		{
			if (Game::IsAudioPlaying(buzzer))Game::StopAudio(buzzer);
			Game::PlayAudio(buzzer, buzzerLoop);
		}
		if (GetHitKey::keys[DIK_3] && !GetHitKey::preKeys[DIK_3])
		{
			Game::togglePrimitiveMode();
		}

		Game::SetMasterVolume(masterVolume);
		//Game::SetAudioVolume(alert, alertVolume);
		//Game::SetAudioVolume(buzzer, buzzerVolume);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Text("-----------------------------------");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("-----------------------------------");
		ImGui::DragFloat3("blockTransforms.scale	", &blockTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.rotate	", &blockTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.pivot	", &blockPivot.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.translate", &blockTransforms.translate.x, 0.01f);
		ImGui::Text("-----------------------------------");
		ImGui::Text("-----------------------------------");




		Game::Drawobj(blockTransforms, blockPivot, blockModel, uvCheckerPng, 0xFFFFFFFFFF);
		AABB aabb = Game::CreateAABB(blockTransforms, blockModel);



		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}