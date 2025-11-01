#include "TestPhase.h"

TestPhase::TestPhase()
{
	uint32_t tex1 = Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	uint32_t tex2 = Game::Resource::LoadTexture("resources/Prototypes/texture/circle.png");
	uint32_t tex3 = Game::Resource::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	uint32_t tex4 = Game::Resource::LoadTexture("resources/Prototypes/texture/white1x1.png");

	uint32_t model1 = Game::Resource::LoadModel("resources/Prototypes/model/", "plane.obj");
	uint32_t model2 = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	uint32_t model3 = Game::Resource::LoadModel("resources/Prototypes/model/", "corn.obj");
	uint32_t model4 = Game::Resource::LoadModel("resources/Prototypes/model/", "sphere.obj");

	audio1 = Game::Resource::LoadAudio("resources/Prototypes/audio/BGM/InGame.mp3");
	audio2 = Game::Resource::LoadAudio("resources/Prototypes/audio/SE/バトル用/氷魔法1.mp3");


	model1_.model = tex1;
	model1_.texture = model1;
	model1_.name = "player";

	model2_.model = tex2;
	model2_.texture = model2;
	model2_.name = "enemy";

	sprite1_.texture = tex1;
	sprite2_.texture = tex3;

	triangle1_.texture = tex1;
	triangle2_.texture = tex1;

	line_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });

	line2_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line2_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line2_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line2_.lineType = LineType::BezierCurve;

	line3_.points.push_back(Vector3{ 10.0f,0.0f,0.0f });
	line3_.points.push_back(Vector3{ 0.0f,10.0f,0.0f });
	line3_.points.push_back(Vector3{ -10.0f,0.0f,0.0f });
	line3_.lineType = LineType::SplineCurve;

	//particle_.model = playerModel;
	//particle_.texture = playerTex;
	particle1_.filePath = "resources/Prototypes/particle/aaa";
	particle1_.LoadJson();

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

		if (GetHitKey::IsPressedDown(DIK_1))
		{
			Game::Audio::PlayAudio(audio1, true);
			if (GetHitKey::IsPressedNow(DIK_LSHIFT))
			{
				Game::Audio::StopAudio(audio1);
			}
		}
		if (GetHitKey::IsPressedDown(DIK_2))
		{
			Game::Audio::PlayAudio(audio2, false);
			if (GetHitKey::IsPressedNow(DIK_LSHIFT))
			{
				Game::Audio::StopAudio(audio2);
			}
		}


		if (ImGui::BeginTabItem("Audio Test"))
		{
			ImGui::Text("Press 1 : play audio1 (hold LSHIFT to stop)");
			ImGui::Text("Press 2 : play audio2 (hold LSHIFT to stop)");
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

			static Vector3 lightDirection = { -1.0f,-1.0f,-1.0f };
			ImGui::DragFloat3("light direction", &lightDirection.x, 0.1f);
			Game::Light::SetLightDirection(lightDirection);

			static float lightIntensity = 1.0f;
			ImGui::SliderFloat("light intensity", &lightIntensity, 0.0f, 10.0f);
			Game::Light::SetLightIntensity(lightIntensity);

			static int lightMode = 0;
			const char* items[] =
			{ "1","3", "4" };
			ImGui::Combo("light mode", &lightMode, items, IM_ARRAYSIZE(items));
			Game::Light::ToggleLightMode(static_cast<uint32_t>(lightMode));

			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region camera test

		if (ImGui::BeginTabItem("Camera Test"))
		{
			static Vector3 cameraCenterTarget;
			static int cameraCenterFrame = 0;
			ImGui::DragFloat3("camera center", &cameraCenterTarget.x, 0.1f);
			ImGui::DragInt("camera center frame", &cameraCenterFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Center"))
			{
				Game::Camera::MoveCameraCenter(cameraCenterTarget, cameraCenterFrame, EaseType::IN_CUBIC);
			}

			static Vector3 cameraRotateTarget;
			static int cameraRotateFrame = 0;
			ImGui::DragFloat3("camera rotate", &cameraRotateTarget.x, 0.1f);
			ImGui::DragInt("camera rotate frame", &cameraRotateFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Rotate"))
			{
				Game::Camera::MoveCameraRotate(cameraRotateTarget, cameraRotateFrame, EaseType::IN_CUBIC);
			}

			static float cameraDistanceTarget = 0.0f;
			static int cameraDistanceFrame = 0;
			ImGui::DragFloat("camera distance", &cameraDistanceTarget, 0.1f);
			ImGui::DragInt("camera distance frame", &cameraDistanceFrame, 1, 0, 600);
			if (ImGui::Button("Set Camera Distance"))
			{
				Game::Camera::MoveCameraDistance(cameraDistanceTarget, cameraDistanceFrame, EaseType::IN_CUBIC);
			}

			static float intensity;
			static float duration;
			static float frequency;
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

		ImGui::EndTabBar();
	}

	ImGui::End();
}


void TestPhase::Draw()
{
	model1_.Draw();
	model1_.DrawAABB();
	model1_.DrawImGui();
	model2_.Draw();
	model2_.DrawAABB();
	model2_.DrawImGui();

	//sprite1_.Draw();
	//sprite1_.DrawImGui();
	//sprite2_.Draw();
	//sprite2_.DrawImGui();

	triangle1_.Draw();
	triangle1_.DrawImGui();
	triangle2_.Draw();
	triangle2_.DrawImGui();

	particle1_.Draw();
	particle1_.DrawImGui();
	particle1_.DrawEmitter();
	particle2_.Draw();
	particle2_.DrawImGui();
	particle2_.DrawEmitter();

	line_.Draw();
	line_.DrawImGui();
	line2_.Draw();
	line2_.DrawImGui();
	line3_.Draw();
	line3_.DrawImGui();
}