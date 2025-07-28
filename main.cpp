#include "Engine/Game.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker checker;
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");


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
	Game::RenderDate_Model block[20][10];
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
	Game::RenderDate_Model skyDome;
	skyDome.model = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	skyDome.texture = Game::LoadTexture("resources/skyDome/skyDome.png");
	skyDome.transforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDome.transforms.translate = { 0.0f,0.0f,0.0f };
	skyDome.transforms.rotate = { 0.0f,0.0f,0.0f };
	skyDome.options.enableWireframeMode = false;

	// テスト
	Game::RenderDate_Model test;
	test.model = Game::LoadOBJ("resources/evaluationTask/", "bunny.obj");
	test.texture = uvCheckerPng;
	test.transforms.scale = { 1.0f, 1.0f, 1.0f };
	test.transforms.translate = { 0.0f,0.0f,0.0f };
	test.transforms.rotate = { 0.0f,0.0f,0.0f };

	block[0][0].transforms.parentWorld = &test.transforms.World;
	skyDome.transforms.parentWorld = &block[0][0].transforms.World;

	// スプライト
	Game::RenderDate_Sprite sprite;
	sprite.texture = uvCheckerPng;
	sprite.transforms.scale = { 0.5f, 0.5f, 0.5f };
	sprite.transforms.rotate = { 0.0f, 0.0f, 0.0f };
	sprite.transforms.translate = { 110.0f, 110.0f, 0.0f };
	sprite.pivot = { 0,0 };


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
		sprite.options.uvTransform.rotate.z += 0.01f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		test.Draw();
		skyDome.Draw();
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 10; ++x)
			{
				block[y][x].Draw();
			}
		}
		sprite.Draw();

		Game::DrawSphere(sphereTransforms, spherePivot, 16, uvCheckerPng, 0xFFFFFFFF, sphereOptions);






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
		ImGui::DragFloat3("test.scale	", &test.transforms.scale.x, 0.01f);
		ImGui::DragFloat3("test.rotate	", &test.transforms.rotate.x, 0.01f);
		ImGui::DragFloat3("test.pivot	", &test.pivot.x, 0.01f);
		ImGui::DragFloat3("test.translate", &test.transforms.translate.x, 0.01f);
		ImGui::DragFloat2("test.uvTransform.scale	", &test.options.uvTransform.scale.x, 0.01f);
		ImGui::DragFloat("test.uvTransform.rotate	", &test.options.uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("test.uvTransform.translate", &test.options.uvTransform.translate.x, 0.01f);
		ImGui::Text("--------------sprite---------------");
		ImGui::DragFloat2("spriteTransforms.scale	 ", &sprite.transforms.scale.x, 0.01f);
		ImGui::DragFloat3("spriteTransforms.rotate	 ", &sprite.transforms.rotate.x, 0.01f);
		ImGui::DragFloat2("spriteTransforms.translate", &sprite.transforms.translate.x, 1.0f);
		ImGui::DragFloat2("uvTransform.spritePivot	", &sprite.pivot.x, 0.1f);
		ImGui::DragFloat2("uvTransform.scale	", &sprite.options.uvTransform.scale.x, 0.01f);
		ImGui::DragFloat("uvTransform.rotate	", &sprite.options.uvTransform.rotate.z, 0.01f);
		ImGui::DragFloat2("uvTransform.translate", &sprite.options.uvTransform.translate.x, 0.01f);


		///
		/// ↑描画処理ここまで
		///



		// フレームの終了
		Game::EndFrame();
	}
	Game::Finalize();


	return 0;
}