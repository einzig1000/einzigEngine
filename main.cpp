#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s
#include "Easings.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"CG2");


	// オーディオデータ
	int alert = Game::LoadAudio("resources/sound/SE/alert.wav");
	int buzzer = Game::LoadAudio("resources/sound/SE/buzzer.mp3");


	// テクスチャ
	int uvCheckerPng;
	uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	int blockPng;
	blockPng = Game::LoadTexture("resources/map.png");
	// モデル
	int blockModel;
	blockModel = Game::LoadOBJ("resources/test", "map.obj");

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

		Game::SetMasterVolume(masterVolume);
		Game::SetAudioVolume(alert, alertVolume);
		Game::SetAudioVolume(buzzer, buzzerVolume);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Text("-----------------------------------");
		ImGui::Text("push 1 key : alert.wav");
		ImGui::Text("push 2 key : buzzer.mp3");
		ImGui::Text("-----------------------------------");

		if (Game::IsAudioPlaying(alert))ImGui::Text("alert ON");
		else ImGui::Text("alert OFF");
		if (Game::IsAudioPlaying(buzzer))ImGui::Text("buzzer ON");
		else ImGui::Text("buzzer OFF");
		ImGui::Text("-----------------------------------");

		ImGui::SliderFloat("masterVolume ", &masterVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("alertVolume  ", &alertVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("buzzerVolume ", &buzzerVolume, 0.0f, 1.0f);
		ImGui::Text("-----------------------------------");

		ImGui::Checkbox("loop alert ", &alertLoop);
		ImGui::Checkbox("loop buzzer", &buzzerLoop);
		ImGui::Text("-----------------------------------");



		//ImGui::
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