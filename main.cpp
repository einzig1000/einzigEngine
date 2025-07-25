#include "Engine/Game.h"

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
	Game::DrawObj block[20][10];
	int blockmodel = Game::LoadOBJ("resources/block", "map.obj");
	int blockTextures = Game::LoadTexture("resources/block/map.png");
	for (int y = 0; y < 20; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			block[y][x].model = blockmodel;
			block[y][x].texture = blockTextures;
			block[y][x].transforms.scale = { 1.2f, 1.2f, 1.2f };
			block[y][x].transforms.translate = { 0.0f + x * 1.5f,0.0f,0.0f + y * 1.5f };
			block[y][x].transforms.rotate = { 0.0f,0.0f,0.0f };
		}
	}

	// 天球
	Game::DrawObj skyDome;
	skyDome.model = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	skyDome.texture = Game::LoadTexture("resources/skyDome/skyDome.png");
	skyDome.transforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDome.transforms.translate = { 0.0f,0.0f,0.0f };
	skyDome.transforms.rotate = { 0.0f,0.0f,0.0f };
	skyDome.options.enableWireframeMode = false;

	// テスト
	Game::DrawObj test;
	//test.model = Game::LoadOBJ("resources/evaluationTask/", "bunny.obj");
	test.texture = uvCheckerPng;
	test.transforms.scale = { 1.0f, 1.0f, 1.0f };
	test.transforms.translate = { 0.0f,0.0f,0.0f };
	test.transforms.rotate = { 0.0f,0.0f,0.0f };
	
	// スプライト
	Transforms spriteTransforms;
	spriteTransforms.scale = { 0.5f, 0.5f, 0.5f };
	spriteTransforms.rotate = { 0.0f, 0.0f, 0.0f };
	spriteTransforms.translate = { 110.0f, 110.0f, 0.0f };
	Vector2 spritePivot = { 0,0 };
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


		//skyDome.options.uvTransform.translate.y += 0.001f;
		//skyDome.options.uvTransform.translate.x += 0.0001f;
		//uvTransform.rotate.z += 0.01f;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		skyDome.Draw();
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 10; ++x)
			{
				block[y][x].Draw();
			}
		}
		Game::DrawSprite(spriteTransforms, spritePivot, uvCheckerPng, 0xFFFFFFFF, uvTransform);

		Game::DrawSphere(sphereTransforms, spherePivot, 16, uvCheckerPng, 0xFFFFFFFF, sphereOptions);


		//test.Draw();




		ImGui::Text("----------------FPS----------------");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("------------Primitive--------------");
		ImGui::Text("push 3 key : WireFrameMode");
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
		ImGui::DragFloat3("blockTransforms.scale	", &block[0][0].transforms.scale.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.rotate	", &block[0][0].transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.pivot	", &block[0][0].pivot.x, 0.01f);
		ImGui::DragFloat3("blockTransforms.translate", &block[0][0].transforms.translate.x, 0.01f);
		ImGui::Text("--------------sphere---------------");
		ImGui::DragFloat3("sphereTransforms.scale	", &sphereTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.rotate	", &sphereTransforms.rotate.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.pivot	", &spherePivot.x, 0.01f);
		ImGui::DragFloat3("sphereTransforms.translate", &sphereTransforms.translate.x, 0.01f);
		ImGui::Text("--------------SkyDome--------------");
		ImGui::DragFloat2("skyDome.uvTransform.scale	", &skyDome.options.uvTransform.scale.x, 0.01f);
		ImGui::DragFloat("skyDome.uvTransform.rotate	", &skyDome.options.uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("skyDome.uvTransform.translate", &skyDome.options.uvTransform.translate.x, 0.01f);
		ImGui::Text("----------------test---------------");
		ImGui::DragFloat2("test.uvTransform.scale	", &test.options.uvTransform.scale.x, 0.01f);
		ImGui::DragFloat("test.uvTransform.rotate	", &test.options.uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("test.uvTransform.translate", &test.options.uvTransform.translate.x, 0.01f);
		ImGui::Text("--------------sprite---------------");
		ImGui::DragFloat2("spriteTransforms.scale	 ", &spriteTransforms.scale.x, 0.01f);
		ImGui::DragFloat3("spriteTransforms.rotate	 ", &spriteTransforms.rotate.x, 0.01f);
		ImGui::DragFloat2("spriteTransforms.translate", &spriteTransforms.translate.x, 1.0f);
		ImGui::DragFloat2("uvTransform.spritePivot	", &spritePivot.x, 0.1f);
		ImGui::DragFloat2("uvTransform.scale	", &uvTransform.scale.x, 0.01f);
		ImGui::DragFloat("uvTransform.rotate	", &uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("uvTransform.translate", &uvTransform.translate.x, 0.01f);
		ImGui::DragFloat2("spriteSize.spriteSize", &spriteSize.x, 1.0f);


		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}