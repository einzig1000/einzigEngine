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
	uint32_t sphereColor = 0xFFFFFFFF;
	Vector3 spherePivot = { 0,0,0 };
	DrawOptions sphereOptions;

	// 天球
	Game::RenderDate_Model skyDome;
	skyDome.model = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	skyDome.texture = Game::LoadTexture("resources/skyDome/skyDome.png");
	skyDome.transforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDome.options.enableWireframeMode = false;

	// ティーポット
	Game::RenderDate_Model teapot;
	teapot.model = Game::LoadOBJ("resources/evaluationTask/", "teapot.obj");
	teapot.texture = uvCheckerPng;
	teapot.transforms.translate = { -3.0f,0.0f,0.0f };
	teapot.pivot = { 3.0f,0.0f,0.0f };

	// ばにー
	Game::RenderDate_Model bunny;
	bunny.model = Game::LoadOBJ("resources/evaluationTask/", "bunny.obj");
	bunny.texture = uvCheckerPng;
	bunny.transforms.translate = { 3.0f,0.0f,0.0f };
	bunny.pivot = { -3.0f,0.0f,0.0f };

	// スザンヌ
	Game::RenderDate_Model suzanne;
	suzanne.model = Game::LoadOBJ("resources/evaluationTask/", "suzanne.obj");
	suzanne.texture = uvCheckerPng;
	suzanne.transforms.translate = { 0.0f,3.0f,0.0f };
	suzanne.pivot = { 0.0f,-3.0f,0.0f };

	// スプライト
	Game::RenderDate_Sprite sprite;
	sprite.texture = uvCheckerPng;
	sprite.transforms.scale = { 0.3f, 0.3f, 0.3f };
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


		skyDome.options.uvTransform.translate.y += 0.001f;
		skyDome.options.uvTransform.translate.x += 0.0001f;
		sprite.options.uvTransform.rotate.z += 0.01f;

		teapot.transforms.rotate.y += 0.01f;
		suzanne.transforms.rotate.y += 0.01f;
		bunny.transforms.rotate.y += 0.01f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		teapot.Draw();
		bunny.Draw();
		suzanne.Draw();
		skyDome.Draw();
		sprite.Draw();


		Game::DrawSphere(sphereTransforms, spherePivot, 16, uvCheckerPng, 0xFFFFFFFF, sphereOptions);






		ImGui::Text("----------------FPS----------------");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("------------Primitive--------------");
		ImGui::Text("push 3 key : WireFrameMode");
		if (ImGui::TreeNode("---------------Audio----------------"))
		{
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

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("--------------Lighting--------------"))
		{
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("light.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			Game::SetLightColor(vector4Color);

			static Vector3 direction = { 0.0f, -1.0f, 0.0f };
			ImGui::DragFloat3("light.direction", &direction.x, 0.01f);
			direction.Normalize();
			Game::SetLightDirection(direction);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------sphere---------------"))
		{
			ImGui::DragFloat3("sphere.scale	", &sphereTransforms.scale.x, 0.01f);
			ImGui::DragFloat3("sphere.rotate	", &sphereTransforms.rotate.x, 0.01f);
			ImGui::DragFloat3("sphere.pivot	", &spherePivot.x, 0.01f);
			ImGui::DragFloat3("sphere.translate", &sphereTransforms.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("sphere.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			sphereColor = ConvertVector4ToUint(vector4Color);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------bunny----------------"))
		{
			ImGui::DragFloat3("bunny.scale	", &bunny.transforms.scale.x, 0.01f);
			ImGui::DragFloat3("bunny.rotate	", &bunny.transforms.rotate.x, 0.01f);
			ImGui::DragFloat3("bunny.pivot	", &bunny.pivot.x, 0.01f);
			ImGui::DragFloat3("bunny.translate", &bunny.transforms.translate.x, 0.01f);
			ImGui::DragFloat2("bunny.uvTransform.scale	", &bunny.options.uvTransform.scale.x, 0.01f);
			ImGui::DragFloat("bunny.uvTransform.rotate	", &bunny.options.uvTransform.rotate.z, 0.01f);
			ImGui::DragFloat2("bunny.uvTransform.translate", &bunny.options.uvTransform.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("bunny.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			bunny.color = ConvertVector4ToUint(vector4Color);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------teapot---------------"))
		{
			ImGui::DragFloat3("teapot.scale	", &teapot.transforms.scale.x, 0.01f);
			ImGui::DragFloat3("teapot.rotate	", &teapot.transforms.rotate.x, 0.01f);
			ImGui::DragFloat3("teapot.pivot	", &teapot.pivot.x, 0.01f);
			ImGui::DragFloat3("teapot.translate", &teapot.transforms.translate.x, 0.01f);
			ImGui::DragFloat2("teapot.uvTransform.scale	", &teapot.options.uvTransform.scale.x, 0.01f);
			ImGui::DragFloat("teapot.uvTransform.rotate	", &teapot.options.uvTransform.rotate.z, 0.01f);
			ImGui::DragFloat2("teapot.uvTransform.translate", &teapot.options.uvTransform.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("teapot.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			teapot.color = ConvertVector4ToUint(vector4Color);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("--------------suzanne---------------"))
		{
			ImGui::DragFloat3("suzanne.scale	", &suzanne.transforms.scale.x, 0.01f);
			ImGui::DragFloat3("suzanne.rotate	", &suzanne.transforms.rotate.x, 0.01f);
			ImGui::DragFloat3("suzanne.pivot	", &suzanne.pivot.x, 0.01f);
			ImGui::DragFloat3("suzanne.translate", &suzanne.transforms.translate.x, 0.01f);
			ImGui::DragFloat2("suzanne.uvTransform.scale	", &suzanne.options.uvTransform.scale.x, 0.01f);
			ImGui::DragFloat("suzanne.uvTransform.rotate	", &suzanne.options.uvTransform.rotate.z, 0.01f);
			ImGui::DragFloat2("suzanne.uvTransform.translate", &suzanne.options.uvTransform.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("suzanne.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			suzanne.color = ConvertVector4ToUint(vector4Color);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------sprite---------------"))
		{
			ImGui::DragFloat2("sprite.scale	 ", &sprite.transforms.scale.x, 0.01f);
			ImGui::DragFloat3("sprite.rotate	 ", &sprite.transforms.rotate.x, 0.01f);
			ImGui::DragFloat2("sprite.translate", &sprite.transforms.translate.x, 1.0f);
			ImGui::DragFloat2("sprite.UVPivot	", &sprite.pivot.x, 0.1f);
			ImGui::DragFloat2("sprite.UVscale	", &sprite.options.uvTransform.scale.x, 0.01f);
			ImGui::DragFloat("sprite.UVrotate	", &sprite.options.uvTransform.rotate.z, 0.01f);
			ImGui::DragFloat2("sprite.UVtranslate", &sprite.options.uvTransform.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("sprite.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			sprite.color = ConvertVector4ToUint(vector4Color);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------SkyDome--------------"))
		{
			ImGui::DragFloat2("skyDome.uvTransform.scale	", &skyDome.options.uvTransform.scale.x, 0.01f);
			ImGui::DragFloat("skyDome.uvTransform.rotate	", &skyDome.options.uvTransform.rotate.z, 0.01f);
			ImGui::DragFloat2("skyDome.uvTransform.translate", &skyDome.options.uvTransform.translate.x, 0.01f);
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