#include "TestPhase.h"

TestPhase::TestPhase()
{
	int32_t tex1 = ResourceID::GetTextureID(TextureID::UVChecker);
	int32_t tex2 = ResourceID::GetTextureID(TextureID::monsterBall);
	int32_t tex3 = ResourceID::GetTextureID(TextureID::Circle);
	int32_t tex4 = ResourceID::GetTextureID(TextureID::white1x1);

	int32_t model1 = ResourceID::GetModelID(ModelID::Cube);
	int32_t model2 = ResourceID::GetModelID(ModelID::Corn);
	int32_t model3 = ResourceID::GetModelID(ModelID::Plane);
	int32_t model4 = ResourceID::GetModelID(ModelID::Sphere);

	audio1 = Game::Resource::LoadAudio("resources/Prototypes/audio/BGM/InGame.mp3");
	audio2 = Game::Resource::LoadAudio("resources/Prototypes/audio/SE/バトル用/氷魔法1.mp3");

	ground_ = new RenderData_Model();
	wall1_ = new RenderData_Model();
	wall2_ = new RenderData_Model();
	wall3_ = new RenderData_Model();
	wall4_ = new RenderData_Model();
	player_ = new RenderData_Model();

	shoulder_ = new RenderData_Model();
	elbow_ = new RenderData_Model();
	hand_ = new RenderData_Model();

	rect_ = new RenderData_Rect();

	sprite1_ = new RenderData_Sprite();
	sprite2_ = new RenderData_Sprite();

	particle1_ = new RenderData_Particle();

	triangle1_ = new RenderData_Triangle();
	triangle2_ = new RenderData_Triangle();

	line_ = new RenderData_Line();
	line2_ = new RenderData_Line();
	line3_ = new RenderData_Line();
		


	ground_->model = model1;
	ground_->texture = tex1;
	ground_->name = "ground";
	ground_->mass = 1001.0f;
	ground_->scale.value = { 10.0f,1.0f,10.0f };

	wall1_->model = model1;
	wall1_->texture = tex1;
	wall1_->name = "wall1";
	wall1_->scale.value = { 0.5f,2.0f,10.0f };
	wall1_->translate.value = { -5.0f,0.5f,0.0f };
	wall1_->mass = 1000.0f;
	wall2_->model = model1;
	wall2_->texture = tex1;
	wall2_->name = "wall2";
	wall2_->scale.value = { 0.5f,2.0f,10.0f };
	wall2_->translate.value = { 5.0f,0.5f,0.0f };
	wall2_->mass = 1000.0f;
	wall3_->model = model1;
	wall3_->texture = tex1;
	wall3_->name = "wall3";
	wall3_->scale.value = { 10.0f,2.0f,0.5f };
	wall3_->translate.value = { 0.0f,0.5f,-5.0f };
	wall3_->mass = 1000.0f;
	wall4_->model = model1;
	wall4_->texture = tex1;
	wall4_->name = "wall4";
	wall4_->scale.value = { 10.0f,2.0f,0.5f };
	wall4_->translate.value = { 0.0f,0.5f,5.0f };
	wall4_->mass = 1000.0f;

	shoulder_->model = model4;
	shoulder_->texture = tex2;
	shoulder_->name = "shoulder";
	shoulder_->translate.value.y = 0.0f;
	elbow_->model = model4;
	elbow_->texture = tex2;
	elbow_->name = "elbow";
	elbow_->translate.value.y = 3.0f;
	hand_->model = model4;
	hand_->texture = tex2;
	hand_->name = "hand";
	hand_->translate.value.y = 6.0f;

	hand_->parentModel = elbow_;
	//elbow_.parentModel = &shoulder_;

	rect_->texture = tex1;
	rect_->pos1 = { 1.0f,1.0f,0.0f };
	rect_->pos2 = { 1.0f,-1.0f,0.0f };
	rect_->pos3 = { -1.0f,1.0f,0.0f };
	rect_->pos4 = { -1.0f,-1.0f,0.0f };

	player_->model = model4;
	player_->texture = tex3;
	player_->name = "player";
	player_->translate.value = { 0.0f,2.0f,0.0f };
	player_->translate.acceleration = { 0.0f,-0.2f,0.0f };
	player_->mass = 1.0f;
	player_->SetBlock(ground_);
	player_->SetBlock(wall1_);
	player_->SetBlock(wall2_);
	player_->SetBlock(wall3_);
	player_->SetBlock(wall4_);

	sprite1_->texture = tex1;
	sprite1_->transforms.scale = { 0.1f,0.1f };
	sprite2_->texture = tex3;
	sprite2_->transforms.scale = { 0.1f,0.1f };

	triangle1_->texture = tex1;
	triangle2_->texture = tex1;

	line_->points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line_->points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line_->points.push_back(Vector3{ -10.0f,0.0f,0.0f });

	line2_->points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line2_->points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line2_->points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line2_->lineType = LineType::BezierCurve;

	line3_->points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line3_->points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line3_->points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line3_->lineType = LineType::SplineCurve;

	particle1_->model = model3;
	particle1_->texture = tex1;
	particle1_->filePath = "resources/Prototypes/particle/aaa";
	//particle1_.LoadJson();


}

TestPhase::~TestPhase()
{
}

void TestPhase::Initialize()
{}


void TestPhase::Update()
{
	ImGui::Begin("TestPhase");

	if (ImGui::BeginTabBar("Facade Test", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable))
	{
#pragma region audio test

		if (ImGui::BeginTabItem("Audio Test"))
		{
			if (ImGui::Button("Play Audio1"))
			{
				Game::Audio::PlayAudio(audio1, true);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Audio1"))
			{
				Game::Audio::StopAudio(audio1);
			}
			if (ImGui::Button("Play Audio2"))
			{
				Game::Audio::PlayAudio(audio2, false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Audio2"))
			{
				Game::Audio::StopAudio(audio2);
			}
			float volume1 = Game::Audio::GetVolume(audio1);
			ImGui::SliderFloat("audio1 volume", &volume1, 0.0f, 1.0f);
			ImGui::Text("audio1 is playing : %d", Game::Audio::IsAudioPlaying(audio1));
			Game::Audio::SetAudioVolume(audio1, volume1);
			float volume2 = Game::Audio::GetVolume(audio2);
			ImGui::SliderFloat("audio2 volume", &volume2, 0.0f, 1.0f);
			ImGui::Text("audio2 is playing : %d", Game::Audio::IsAudioPlaying(audio2));
			Game::Audio::SetAudioVolume(audio2, volume2);
			float masterVolume = Game::Audio::GetMasterVolume();
			ImGui::SliderFloat("master volume", &masterVolume, 0.0f, 1.0f);
			Game::Audio::SetMasterVolume(masterVolume);
			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region light test

		if (ImGui::BeginTabItem("Light Test"))
		{
			static Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
			ImGui::ColorEdit4("light color", &lightColor.x, 1);
			Game::Light::SetLightColor(lightColor);

			static Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
			lightDirection.Normalize();
			ImGui::DragFloat3("light direction", &lightDirection.x, 0.1f);
			Game::Light::SetLightDirection(lightDirection);

			static float lightIntensity = 1.0f;
			ImGui::SliderFloat("light intensity", &lightIntensity, 0.0f, 10.0f);
			Game::Light::SetLightIntensity(lightIntensity);
	
			static int current_item = 0;
			static const char* items[] = { "None", "Lambert", "HalfLambert" };
			if (ImGui::Combo("dirLight.shadingType", &current_item, items, IM_ARRAYSIZE(items)))
			{
				LightMode mode = LightMode::None;
				if (current_item == 0)
				{
					mode = LightMode::None;
				}
				else if (current_item == 1)
				{
					mode = LightMode::Lambert;
				}
				else if (current_item == 2)
				{
					mode = LightMode::HalfLambert;
				}
				Game::Light::ToggleLightMode(mode);
			}



			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region camera test

		if (ImGui::BeginTabItem("Camera Test"))
		{
			static Vector3 cameraCenterTarget;
			static int cameraCenterFrame = 120;
			ImGui::DragFloat3("camera center", &cameraCenterTarget.x, 0.1f);
			ImGui::DragInt("camera center frame", &cameraCenterFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Center"))
			{
				Game::Camera::MoveCameraCenter(cameraCenterTarget, cameraCenterFrame, EaseType::IN_CUBIC);
			}

			static Vector3 cameraRotateTarget;
			static int cameraRotateFrame = 120;
			ImGui::DragFloat3("camera rotate", &cameraRotateTarget.x, 0.1f);
			ImGui::DragInt("camera rotate frame", &cameraRotateFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Rotate"))
			{
				Game::Camera::MoveCameraRotate(cameraRotateTarget, cameraRotateFrame, EaseType::IN_CUBIC);
			}

			static float cameraDistanceTarget = 0.0f;
			static int cameraDistanceFrame = 120;
			ImGui::DragFloat("camera distance", &cameraDistanceTarget, 0.1f);
			ImGui::DragInt("camera distance frame", &cameraDistanceFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Distance"))
			{
				Game::Camera::MoveCameraDistance(cameraDistanceTarget, cameraDistanceFrame, EaseType::IN_CUBIC);
			}

			static float intensity = 3.0f;
			static float duration = 35.0f;
			static float frequency = 25.0f;
			ImGui::DragFloat("camera shake intensity", &intensity, 0.1f);
			ImGui::DragFloat("camera shake duration", &duration, 0.1f);
			ImGui::DragFloat("camera shake frequency", &frequency, 0.1f);
			if (ImGui::Button("Start Camera Shake"))
			{
				Game::Camera::StartCameraShake(intensity, duration, frequency);
			}
			ImGui::Text("is camera shaking : %d", Game::Camera::IsCameraShaking());
			if (ImGui::Button("Stop Camera Shake"))
			{
				Game::Camera::StopCameraShake();
			}

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region mouse test

		if (ImGui::BeginTabItem("mouse Test"))
		{
			ImGui::Text("Mouse Position: (%.1f, %.1f)", Game::Input::Mouse::GetMousePosition().x, Game::Input::Mouse::GetMousePosition().y);
			ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetMouseWorldPosition().x, Game::Input::Mouse::GetMouseWorldPosition().y, Game::Input::Mouse::GetMouseWorldPosition().z);
			ImGui::Text("Mouse Ray Origin: (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetMouseRay().origin.x, Game::Input::Mouse::GetMouseRay().origin.y, Game::Input::Mouse::GetMouseRay().origin.z);
			ImGui::Text("Mouse Ray Diff  : (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetMouseRay().diff.x, Game::Input::Mouse::GetMouseRay().diff.y, Game::Input::Mouse::GetMouseRay().diff.z);
			ImGui::Text("Mouse Wheel: %d", Game::Input::Mouse::GetMouseWheel());

			ImGui::Text("Mouse Buttons:");
			ImGui::Text("Left Button - %d-%d-%d : %d",
				Game::Input::Mouse::IsJustPressed(0),
				Game::Input::Mouse::IsHeld(0),
				Game::Input::Mouse::IsJustReleased(0),
				Game::Input::Mouse::HoldFrames(0));
			ImGui::Text("Right Button - %d-%d-%d : %d",
				Game::Input::Mouse::IsJustPressed(1),
				Game::Input::Mouse::IsHeld(1),
				Game::Input::Mouse::IsJustReleased(1),
				Game::Input::Mouse::HoldFrames(1));
			ImGui::Text("Middle Button - %d-%d-%d : %d",
				Game::Input::Mouse::IsJustPressed(2),
				Game::Input::Mouse::IsHeld(2),
				Game::Input::Mouse::IsJustReleased(2),
				Game::Input::Mouse::HoldFrames(2));

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region keyboard test

		if (ImGui::BeginTabItem("keyboard Test"))
		{
			struct KeyInfo {
				const char* name;
				int dik;
			};

			static const KeyInfo kKeys[] = {
				{"A", DIK_A}, {"B", DIK_B}, {"C", DIK_C}, {"D", DIK_D},
				{"E", DIK_E}, {"F", DIK_F}, {"G", DIK_G}, {"H", DIK_H},
				{"I", DIK_I}, {"J", DIK_J}, {"K", DIK_K}, {"L", DIK_L},
				{"M", DIK_M}, {"N", DIK_N}, {"O", DIK_O}, {"P", DIK_P},
				{"Q", DIK_Q}, {"R", DIK_R}, {"S", DIK_S}, {"T", DIK_T},
				{"U", DIK_U}, {"V", DIK_V}, {"W", DIK_W}, {"X", DIK_X},
				{"Y", DIK_Y}, {"Z", DIK_Z},

				{"Space", DIK_SPACE},

				{"F1", DIK_F1}, {"F2", DIK_F2}, {"F3", DIK_F3}, {"F4", DIK_F4},
				{"F5", DIK_F5}, {"F6", DIK_F6}, {"F7", DIK_F7}, {"F8", DIK_F8},
				{"F9", DIK_F9}, {"F10", DIK_F10}, {"F11", DIK_F11}, {"F12", DIK_F12},

				{"Enter", DIK_RETURN}, {"Escape", DIK_ESCAPE},

				{"Up", DIK_UP}, {"Down", DIK_DOWN}, {"Left", DIK_LEFT}, {"Right", DIK_RIGHT},

				{"LShift", DIK_LSHIFT}, {"RShift", DIK_RSHIFT},

				{"0", DIK_0}, {"1", DIK_1}, {"2", DIK_2}, {"3", DIK_3}, {"4", DIK_4},
				{"5", DIK_5}, {"6", DIK_6}, {"7", DIK_7}, {"8", DIK_8}, {"9", DIK_9}
			};

			for (const auto& k : kKeys) {
				if (Game::Input::Key::IsHeld(k.dik) ||
					Game::Input::Key::IsJustPressed(k.dik) ||
					Game::Input::Key::IsJustReleased(k.dik))
				{
					ImGui::Text("%s : %d:%d:%d _ %d",
						k.name,
						Game::Input::Key::IsJustPressed(k.dik),
						Game::Input::Key::IsHeld(k.dik),
						Game::Input::Key::IsJustReleased(k.dik),
						Game::Input::Key::HoldFrames(k.dik)
					);
				}
			}
			ImGui::EndTabItem();
		}

#pragma endregion

		ImGui::EndTabBar();
	}

	ImGui::End();

	if (Game::Input::Key::IsHeld(DIK_A))
	{
		player_->translate.value.x -= 0.1f;
	}
	if (Game::Input::Key::IsHeld(DIK_D))
	{
		player_->translate.value.x += 0.1f;
	}
	if (Game::Input::Key::IsHeld(DIK_S))
	{
		player_->translate.value.z -= 0.1f;
	}
	if (Game::Input::Key::IsHeld(DIK_W))
	{
		player_->translate.value.z += 0.1f;
	}
	if (Game::Input::Key::IsJustPressed(DIK_SPACE))
	{
		player_->translate.velocity.y += 2.5f;
	}
}


void TestPhase::Draw()
{
	rect_->Draw();
	rect_->DrawImGui();
	
	ground_->Draw();
	ground_->DrawImGui();
	ground_->DrawAABB();
	wall1_->Draw();
	wall2_->Draw();
	wall3_->Draw();
	wall4_->Draw();

	player_->Draw();
	player_->DrawAABB();
	player_->DrawImGui();

	sprite1_->Draw();
	sprite1_->DrawImGui();
	sprite2_->Draw();
	sprite2_->DrawImGui();

	triangle1_->Draw();
	triangle1_->DrawImGui();
	triangle2_->Draw();
	triangle2_->DrawImGui();

	particle1_->Draw();
	particle1_->DrawEmitter();
	particle1_->DrawImGui();
	particle1_->DrawEmitter();

	line_->Draw();
	line_->DrawImGui();
	line2_->Draw();
	line2_->DrawImGui();
	line3_->Draw();
	line3_->DrawImGui();

	shoulder_->Draw();
	shoulder_->DrawImGui();
	
	elbow_->Draw();
	elbow_->DrawImGui();
	hand_->Draw();
	hand_->DrawImGui();
}