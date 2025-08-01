#include "Engine/Game.h"
#include <numbers>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(WIDTH, HEIGHT, L"LE2A_17_ヨコヤマ_タダノブ");


	int uvCheckerPng = Game::LoadTexture("resources/uvChecker.png");

	const char* objectName[] = { "nothing","sphere", "teapot", "bunny", "suzanne", "multiMesh" };

	// 球
	Game::RenderData_Model sphere;
	sphere.model = Game::LoadOBJ("resources/evaluationTask", "sphere.obj");
	sphere.texture = uvCheckerPng;
	sphere.transforms.scale = { 2.0f, 2.0f, 2.0f };

	// 天球
	Game::RenderData_Model skyDome;
	skyDome.model = Game::LoadOBJ("resources/skyDome", "skyDome.obj");
	skyDome.texture = Game::LoadTexture("resources/skyDome/skyDome.png");
	skyDome.transforms.scale = { 120.0f, 120.0f, 120.0f };
	skyDome.options.enableWireframeMode = false;

	// ティーポット
	Game::RenderData_Model teapot;
	teapot.model = Game::LoadOBJ("resources/evaluationTask/", "teapot.obj");
	teapot.texture = uvCheckerPng;
	teapot.transforms.translate = { -3.0f,0.0f,0.0f };
	teapot.transforms.parentWorld = &sphere.transforms.World;

	// ばにー
	Game::RenderData_Model bunny;
	bunny.model = Game::LoadOBJ("resources/evaluationTask/", "bunny.obj");
	bunny.texture = uvCheckerPng;
	bunny.transforms.translate = { 3.0f,0.0f,0.0f };
	bunny.transforms.parentWorld = &sphere.transforms.World;

	// スザンヌ
	Game::RenderData_Model suzanne;
	suzanne.model = Game::LoadOBJ("resources/evaluationTask/", "suzanne.obj");
	suzanne.texture = uvCheckerPng;
	suzanne.transforms.translate = { 0.0f,3.0f,0.0f };
	suzanne.transforms.parentWorld = &sphere.transforms.World;

	// マルチメッシュ
	Game::RenderData_Model multiMesh;
	multiMesh.model = Game::LoadOBJ("resources/evaluationTask/", "multiMesh.obj");
	multiMesh.texture = uvCheckerPng;
	multiMesh.transforms.translate = { -8.0f,0.0f,0.0f };
	multiMesh.transforms.rotate = { 0.0f, float(std::numbers::pi), 0.0f };

	// マルチマテリアル
	Game::RenderData_Model multiMaterial;
	multiMaterial.model = Game::LoadOBJ("resources/evaluationTask/", "multiMaterial.obj");
	multiMaterial.texture = uvCheckerPng;
	multiMaterial.transforms.translate = { 8.0f,0.0f,0.0f };
	multiMaterial.transforms.rotate = { 0.0f, float(std::numbers::pi), 0.0f };

	// スプライト
	Game::RenderData_Sprite sprite;
	sprite.texture = uvCheckerPng;
	sprite.transforms.scale = { 0.3f, 0.3f, 0.3f };
	sprite.transforms.rotate = { 0.0f, 0.0f, 0.0f };
	sprite.transforms.translate = { 110.0f, 110.0f, 0.0f };
	sprite.pivot = { 0,0 };


	// オーディオ
	int alert = Game::LoadAudio("resources/sound/SE/alert.wav");
	int buzzer = Game::LoadAudio("resources/sound/SE/buzzer.mp3");
	float masterVolume = Game::GetMasterVolume();
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




		if ((GetHitKey::keys[DIK_1] && !GetHitKey::preKeys[DIK_1]) || (GetPadState::buttons[PAD_A] && !GetPadState::preButtons[PAD_A]))
		{
			if (Game::IsAudioPlaying(alert))Game::StopAudio(alert);
			Game::PlayAudio(alert, alertLoop);
		}
		if ((GetHitKey::keys[DIK_2] && !GetHitKey::preKeys[DIK_2]) || (GetPadState::buttons[PAD_B] && !GetPadState::preButtons[PAD_B]))
		{
			if (Game::IsAudioPlaying(buzzer))Game::StopAudio(buzzer);
			Game::PlayAudio(buzzer, buzzerLoop);
		}
		if ((GetHitKey::keys[DIK_3] && !GetHitKey::preKeys[DIK_3]) || (GetPadState::buttons[PAD_X] && !GetPadState::preButtons[PAD_X]))
		{
			Game::toggleWireframeMode();
		}

		Game::SetMasterVolume(masterVolume);
		Game::SetAudioVolume(alert, alertVolume);
		Game::SetAudioVolume(buzzer, buzzerVolume);


		skyDome.options.uvTransform.translate.y += 0.001f;
		skyDome.options.uvTransform.translate.x += 0.0001f;
		sprite.options.uvTransform.rotate.z += 0.01f;
		
		teapot.transforms.rotate.x += 0.01f;
		suzanne.transforms.rotate.y += 0.01f;
		bunny.transforms.rotate.z += 0.01f;

		sphere.transforms.rotate.y += 0.01f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		skyDome.Draw();
		sprite.Draw();
		teapot.Draw();
		bunny.Draw();
		bunny.DrawAABB();
		suzanne.Draw();
		sphere.Draw();
		multiMesh.Draw();
		multiMaterial.Draw();

		sphere.DrawAABB();

		




		ImGui::Text("----------------FPS----------------");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("------------Primitive--------------");
		ImGui::Text("push 3 key    : WireFrameMode");
		ImGui::Text("push X Button : WireFrameMode");
		if (ImGui::TreeNode("---------------Audio----------------"))
		{
			ImGui::Text("push 1 key    : alert.wav");
			ImGui::Text("push A Button : alert.wav");
			ImGui::Text("push 2 key    : buzzer.mp3");
			ImGui::Text("push B Button : buzzer.mp3");
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
			ImGui::ColorEdit4("Light.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			Game::SetLightColor(vector4Color);

			static Vector3 direction = { 0.0f, -1.0f, 0.0f };
			ImGui::DragFloat3("Light.direction", &direction.x, 0.01f);
			direction.Normalize();
			Game::SetLightDirection(direction);

			static float intensity = 1.0f;
			ImGui::DragFloat("Light.intensity", &intensity, 0.01f);
			Game::SetLightIntensity(intensity);

			static const char* modeName[] = { "Half Lambert", "Lambert","Nothig" };
			static int current_mode = 0;
			ImGui::Combo("LightingMode", &current_mode, modeName, IM_ARRAYSIZE(modeName));
			Game::ToggleLightMode(current_mode);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("---------------bunny----------------"))
		{
			static int current_parent = 1;
			ImGui::Combo("parentObject", &current_parent, objectName, IM_ARRAYSIZE(objectName));
			if (current_parent == 0)
			{
				bunny.transforms.parentWorld = nullptr;
			}
			else if (current_parent == 1)
			{
				bunny.transforms.parentWorld = &sphere.transforms.World;
			}
			else if (current_parent == 2)
			{
				bunny.transforms.parentWorld = &teapot.transforms.World;
			}
			else if (current_parent == 3)
			{
				bunny.transforms.parentWorld = &bunny.transforms.World;
			}
			else if (current_parent == 4)
			{
				bunny.transforms.parentWorld = &suzanne.transforms.World;
			}
			else if (current_parent == 5)
			{
				bunny.transforms.parentWorld = &multiMesh.transforms.World;
			}

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
			static int current_parent = 1;
			ImGui::Combo("parentObject", &current_parent, objectName, IM_ARRAYSIZE(objectName));
			if (current_parent == 0)
			{
				teapot.transforms.parentWorld = nullptr;
			}
			else if (current_parent == 1)
			{
				teapot.transforms.parentWorld = &sphere.transforms.World;
			}
			else if (current_parent == 2)
			{
				teapot.transforms.parentWorld = &teapot.transforms.World;
			}
			else if (current_parent == 3)
			{
				teapot.transforms.parentWorld = &bunny.transforms.World;
			}
			else if (current_parent == 4)
			{
				teapot.transforms.parentWorld = &suzanne.transforms.World;
			}
			else if (current_parent == 5)
			{
				teapot.transforms.parentWorld = &multiMesh.transforms.World;
			}
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
			static int current_parent = 1;
			ImGui::Combo("parentObject", &current_parent, objectName, IM_ARRAYSIZE(objectName));
			if (current_parent == 0)
			{
				suzanne.transforms.parentWorld = nullptr;
			}
			else if (current_parent == 1)
			{
				suzanne.transforms.parentWorld = &sphere.transforms.World;
			}
			else if (current_parent == 2)
			{
				suzanne.transforms.parentWorld = &teapot.transforms.World;
			}
			else if (current_parent == 3)
			{
				suzanne.transforms.parentWorld = &bunny.transforms.World;
			}
			else if (current_parent == 4)
			{
				suzanne.transforms.parentWorld = &suzanne.transforms.World;
			}
			else if (current_parent == 5)
			{
				suzanne.transforms.parentWorld = &multiMesh.transforms.World;
			}
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
		if (ImGui::TreeNode("---------------sphere---------------"))
		{
			ImGui::DragFloat3("sphere.scale	", &sphere.transforms.scale.x, 0.01f);
			ImGui::DragFloat3("sphere.rotate	", &sphere.transforms.rotate.x, 0.01f);
			ImGui::DragFloat3("sphere.pivot	", &sphere.pivot.x, 0.01f);
			ImGui::DragFloat3("sphere.translate", &sphere.transforms.translate.x, 0.01f);
			static float floatColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			ImGui::ColorEdit4("sphere.color", floatColor, 1);
			Vector4 vector4Color = { floatColor[0], floatColor[1], floatColor[2], floatColor[3] };
			sphere.color = ConvertVector4ToUint(vector4Color);
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