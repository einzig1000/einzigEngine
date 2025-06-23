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


	// 球
	int uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");
	Transforms sphereTransforms;
	sphereTransforms.scale = { 1.0f, 1.0f, 1.0f };
	sphereTransforms.translate = { 0.0f,0.0f,0.0f };
	sphereTransforms.rotate = { 0.0f,0.0f,0.0f };
	Vector3 spherePivot = { 0,0,0 };
	DrawOptions sphereOptions;
	// ブロック
	int blockModel = Game::LoadOBJ("resources/block", "map.obj");
	int blockPng = Game::LoadTexture("resources/block/map.png");
	Transforms blockTransforms;
	blockTransforms.scale = { 1.2f, 1.2f, 1.2f };
	blockTransforms.translate = { 0.0f,0.0f,0.0f };
	blockTransforms.rotate = { 0.0f,0.0f,0.0f };
	Vector3 blockPivot = { 0,0,0 };
	DrawOptions blockOptions;
	// 天球
	int skyDomeModel = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	int skyDomePng = Game::LoadTexture("resources/skyDome/skyDome.png");
	Transforms skyDomeTransforms;
	skyDomeTransforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDomeTransforms.translate = { 0.0f,0.0f,0.0f };
	skyDomeTransforms.rotate = { 0.0f,0.0f,0.0f };
	DrawOptions skyDomeOptions;
	skyDomeOptions.enableWireframeMode = false;
	// スプライト
	Transforms spriteTransforms;
	spriteTransforms.scale = { 1.0f, 1.0f, 1.0f };
	spriteTransforms.rotate = { 0.0f, 0.0f, 0.0f };
	spriteTransforms.translate = { 0.0f, 0.0f, 0.0f };
	Transforms uvTransform;


	// 現在のマスター音量
	float masterVolume = Game::GetMasterVolume();
	// 現在のそれぞれの音量
	float alertVolume = Game::GetVolume(alert);
	float buzzerVolume = Game::GetVolume(buzzer);

	bool alertLoop = false;
	bool buzzerLoop = false;


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
			Game::toggleWireframeMode();
		}

		Game::SetMasterVolume(masterVolume);
		Game::SetAudioVolume(alert, alertVolume);
		Game::SetAudioVolume(buzzer, buzzerVolume);


		skyDomeOptions.uvTransform.translate.y += 0.001f;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Game::Drawobj(skyDomeTransforms, {0,0,0}, skyDomeModel, skyDomePng, 0xFFFFFFFFFF, skyDomeOptions);
		Game::Drawobj(blockTransforms, blockPivot, blockModel, blockPng, 0xFFFFFFFFFF, blockOptions);
		Game::DrawSphere(sphereTransforms, spherePivot, 12, uvCheckerPng, 0xFFFFFFFFFF, sphereOptions);
		Game::DrawSprite(spriteTransforms, uvCheckerPng, 0xFFFFFFFF, uvTransform);







		ImGui::Text("----------------FPS----------------");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("---------------Audio---------------");
		ImGui::Text("push 1 key : alert.wav");
		ImGui::Text("push 2 key : buzzer.mp3");
		if (Game::IsAudioPlaying(alert))ImGui::Text("alert ON");
		else ImGui::Text("alert OFF");
		if (Game::IsAudioPlaying(buzzer))ImGui::Text("buzzer ON");
		else ImGui::Text("buzzer OFF");
		ImGui::SliderFloat("masterVolume ", &masterVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("alertVolume  ", &alertVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("buzzerVolume ", &buzzerVolume, 0.0f, 1.0f);
		ImGui::Checkbox("loop alert ", &alertLoop);
		ImGui::Checkbox("loop buzzer", &buzzerLoop);
		ImGui::Text("---------------block---------------");
		ImGui::DragFloat3("blockTransforms.scale	", &blockTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.rotate	", &blockTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.pivot	", &blockPivot.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.translate", &blockTransforms.translate.x, 0.01f);
		ImGui::Text("--------------sphere---------------");
		ImGui::DragFloat3("sphereTransforms.scale	", &sphereTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.rotate	", &sphereTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.pivot	", &spherePivot.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.translate", &sphereTransforms.translate.x, 0.01f);
		ImGui::Text("--------------SkyDome--------------");
		ImGui::DragFloat2("skyDome.uvTransform.scale	", &skyDomeOptions.uvTransform.scale.x, 0.01f);
		ImGui::DragFloat ("skyDome.uvTransform.rotate	", &skyDomeOptions.uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("skyDome.uvTransform.translate", &skyDomeOptions.uvTransform.translate.x, 0.01f);



		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}