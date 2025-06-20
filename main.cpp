#include "Novice.h"
#include "functions.h"
#include "Easings.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//D3DResourceLeakChecker checker_;
	//Game game_;

	//checker = &checker_;
	//game = &game_;



	// ウィンドウ、DrectX初期化
	Novice::Initialize(WIDTH, HEIGHT, L"CG2");


	// オーディオデータ
	int alert = Novice::LoadAudio("resources/sound/SE/alert.wav");
	int buzzer = Novice::LoadAudio("resources/sound/SE/buzzer.mp3");


	// テクスチャ
	int uvCheckerPng;
	uvCheckerPng = Novice::LoadTexture("resources/uvChecker.png");
	int blockPng;
	blockPng = Novice::LoadTexture("resources/map.png");
	// モデル
	int blockModel;
	blockModel = Novice::LoadOBJ("resources/model", "map.obj");

	// 現在のマスター音量
	float masterVolume = Novice::GetMasterVolume();
	// 現在のそれぞれの音量
	float alertVolume = Novice::GetVolume(alert);
	float buzzerVolume = Novice::GetVolume(buzzer);

	bool alertLoop = false;
	bool buzzerLoop = false;

	Transforms blockTransforms;
	blockTransforms.scale = { 1.2f, 1.2f, 1.2f };
	blockTransforms.translate = { 0,0,0 };
	blockTransforms.rotate = { 0.0f,0.0f,0.0f };

	while (Novice::ProcessMessage())
	{
		// フレームの開始
		Novice::BeginFrame();
		Novice::SetMouseRay();


		///
		/// ↓更新処理ここから
		///
		if (GetHitKey::keys[DIK_1] && !GetHitKey::preKeys[DIK_1])
		{
			if (Novice::IsAudioPlaying(alert))Novice::StopAudio(alert);
			Novice::PlayAudio(alert, alertLoop);
		}
		if (GetHitKey::keys[DIK_2] && !GetHitKey::preKeys[DIK_2])
		{
			if (Novice::IsAudioPlaying(buzzer))Novice::StopAudio(buzzer);
			Novice::PlayAudio(buzzer, buzzerLoop);
		}

		Novice::SetMasterVolume(masterVolume);
		Novice::SetAudioVolume(alert, alertVolume);
		Novice::SetAudioVolume(buzzer, buzzerVolume);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Text("-----------------------------------");
		ImGui::Begin("FPS");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Text("-----------------------------------");




		Novice::Drawobj(blockTransforms, {0,0,0}, blockModel, uvCheckerPng, 0xFFFFFFFFFF);
		AABB aabb = Novice::CreateAABB(blockTransforms, blockModel);

		Novice::DrawLine({ aabb.min.x, aabb.min.y, aabb.min.z }, { aabb.max.x, aabb.min.y, aabb.min.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.max.y, aabb.min.z }, { aabb.max.x, aabb.max.y, aabb.min.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.min.y, aabb.max.z }, { aabb.max.x, aabb.min.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.max.y, aabb.max.z }, { aabb.max.x, aabb.max.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.min.y, aabb.min.z }, { aabb.min.x, aabb.max.y, aabb.min.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.max.x, aabb.min.y, aabb.min.z }, { aabb.max.x, aabb.max.y, aabb.min.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.min.y, aabb.max.z }, { aabb.min.x, aabb.max.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.max.x, aabb.min.y, aabb.max.z }, { aabb.max.x, aabb.max.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.min.y, aabb.min.z }, { aabb.min.x, aabb.min.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.max.x, aabb.min.y, aabb.min.z }, { aabb.max.x, aabb.min.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.min.x, aabb.max.y, aabb.min.z }, { aabb.min.x, aabb.max.y, aabb.max.z }, 0xFFFFFFFF);
		Novice::DrawLine({ aabb.max.x, aabb.max.y, aabb.min.z }, { aabb.max.x, aabb.max.y, aabb.max.z }, 0xFFFFFFFF);


		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Novice::EndFrame();
	}
	Novice::Finalize();


	return 0;
}