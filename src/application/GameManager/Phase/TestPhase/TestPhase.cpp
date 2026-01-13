#include "GameManager/Phase/TestPhase/TestPhase.h"

TestPhase::TestPhase()
{
	int32_t tex1 = ResourceID::GetTextureID(TextureID::UVChecker);
	int32_t tex2 = ResourceID::GetTextureID(TextureID::monsterBall);
	int32_t tex3 = ResourceID::GetTextureID(TextureID::Circle);
	int32_t tex4 = ResourceID::GetTextureID(TextureID::white1x1);
	int32_t tex5 = ResourceID::GetTextureID(TextureID::font_0);

	int32_t model1 = ResourceID::GetModelID(ItemID::ダイヤのツルハシ);
	int32_t model2 = ResourceID::GetModelID(ItemID::ダイヤの剣);
	int32_t model3 = ResourceID::GetModelID(ItemID::ダイヤの斧);
	int32_t model4 = ResourceID::GetModelID(ItemID::金インゴット);

	audio1 = Game::Resource::LoadAudio("resources/Prototypes/audio/BGM/InGame.mp3");
	audio2 = Game::Resource::LoadAudio("resources/Prototypes/audio/SE/バトル用/氷魔法1.mp3");


	ground_ = std::make_unique<RenderData_Model>();
	wall1_ = std::make_unique<RenderData_Model>();
	wall2_ = std::make_unique<RenderData_Model>();
	wall3_ = std::make_unique<RenderData_Model>();
	wall4_ = std::make_unique<RenderData_Model>();
	player_ = std::make_unique<RenderData_Model>();

	shoulder_ = std::make_unique<RenderData_Model>();
	elbow_ = std::make_unique<RenderData_Model>();
	hand_ = std::make_unique<RenderData_Model>();

	rect_ = std::make_unique<RenderData_Rect>();

	sprite1_ = std::make_unique<RenderData_Sprite>();
	sprite2_ = std::make_unique<RenderData_Sprite>();

	particle1_ = std::make_unique<RenderData_Particle>();

	triangle1_ = std::make_unique<RenderData_Triangle>();
	triangle2_ = std::make_unique<RenderData_Triangle>();

	line_ = std::make_unique<RenderData_Line>();
	line2_ = std::make_unique<RenderData_Line>();
	line3_ = std::make_unique<RenderData_Line>();

	ground_->SetModel(model1);
	ground_->SetTexture(tex1);
	ground_->name = "ground";
	ground_->mass = 1001.0f;
	ground_->scale.value = { 10.0f,1.0f,10.0f };

	wall1_->SetModel(model1);
	wall1_->SetTexture(tex1);
	wall1_->name = "wall1";
	wall1_->scale.value = { 0.5f,2.0f,10.0f };
	wall1_->translate.value = { -5.0f,0.5f,0.0f };
	wall1_->mass = 1000.0f;
	wall2_->SetModel(model1);
	wall2_->SetTexture(tex1);
	wall2_->name = "wall2";
	wall2_->scale.value = { 0.5f,2.0f,10.0f };
	wall2_->translate.value = { 5.0f,0.5f,0.0f };
	wall2_->mass = 1000.0f;
	wall3_->SetModel(model1);
	wall3_->SetTexture(tex1);
	wall3_->name = "wall3";
	wall3_->scale.value = { 10.0f,2.0f,0.5f };
	wall3_->translate.value = { 0.0f,0.5f,-5.0f };
	wall3_->mass = 1000.0f;
	wall4_->SetModel(model1);
	wall4_->SetTexture(tex1);
	wall4_->name = "wall4";
	wall4_->scale.value = { 10.0f,2.0f,0.5f };
	wall4_->translate.value = { 0.0f,0.5f,5.0f };
	wall4_->mass = 1000.0f;

	shoulder_->SetModel(model4);
	shoulder_->SetTexture(tex2);
	shoulder_->name = "shoulder";
	shoulder_->translate.value.y = 0.0f;
	elbow_->SetModel(model4);
	elbow_->SetTexture(tex2);
	elbow_->name = "elbow";
	elbow_->translate.value.y = 3.0f;
	hand_->SetModel(model4);
	hand_->SetTexture(tex2);
	hand_->name = "hand";
	hand_->translate.value.y = 6.0f;

	hand_->parentModel = elbow_.get();
	elbow_->parentModel = shoulder_.get();

	rect_->texture = tex1;
	rect_->pos1 = { 1.0f,1.0f,0.0f };
	rect_->pos2 = { 1.0f,-1.0f,0.0f };
	rect_->pos3 = { -1.0f,1.0f,0.0f };
	rect_->pos4 = { -1.0f,-1.0f,0.0f };

	player_->SetModel(model4);
	player_->SetTexture(tex3);
	player_->name = "player";
	player_->translate.value = { 0.0f,2.0f,0.0f };
	player_->translate.acceleration = { 0.0f,-0.2f,0.0f };
	player_->mass = 1.0f;
	player_->SetBlock(ground_.get());
	player_->SetBlock(wall1_.get());
	player_->SetBlock(wall2_.get());
	player_->SetBlock(wall3_.get());
	player_->SetBlock(wall4_.get());

	sprite1_->texture = tex5;
	sprite1_->transforms.scale = { 0.1f,0.1f };
	sprite1_->anchor = Anchor::CenterLeft;
	sprite1_->cutImageSize = Vector2int(32, 32);
	sprite1_->cutImageLeftTop = fontCutImagePos('0');

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

}


TestPhase::~TestPhase()
{
}


void TestPhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;
}


void TestPhase::Update()
{
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
	
	ground_->Draw();
	ground_->DrawAABB();
	wall1_->Draw();
	wall2_->Draw();
	wall3_->Draw();
	wall4_->Draw();

	player_->Draw();
	player_->DrawAABB();

	sprite1_->Draw();
	sprite2_->Draw();

	triangle1_->Draw();
	triangle2_->Draw();

	particle1_->Draw();
	particle1_->DrawEmitter();

	line_->Draw();
	line2_->Draw();
	line3_->Draw();

	shoulder_->Draw();
	elbow_->Draw();
	hand_->Draw();
}


void TestPhase::DrawImGui()
{
	//rect_->DrawImGui();
	//
	//ground_->DrawImGui();
	//
	//player_->DrawImGui();
	//
	//sprite1_->DrawImGui();
	//sprite2_->DrawImGui();
	//
	//triangle1_->DrawImGui();
	//triangle2_->DrawImGui();
	//
	//particle1_->DrawImGui();
	//
	//line_->DrawImGui();
	//line2_->DrawImGui();
	//line3_->DrawImGui();
	//
	//shoulder_->DrawImGui();
	//elbow_->DrawImGui();
	//hand_->DrawImGui();

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
			ImGui::Text("Mouse Position: (%.1f, %.1f)", Game::Input::Mouse::GetPosition().x, Game::Input::Mouse::GetPosition().y);
			ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetWorldPosition().x, Game::Input::Mouse::GetWorldPosition().y, Game::Input::Mouse::GetWorldPosition().z);
			ImGui::Text("Mouse Ray Origin: (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetRay().origin.x, Game::Input::Mouse::GetRay().origin.y, Game::Input::Mouse::GetRay().origin.z);
			ImGui::Text("Mouse Ray Diff  : (%.1f, %.1f, %.1f)", Game::Input::Mouse::GetRay().diff.x, Game::Input::Mouse::GetRay().diff.y, Game::Input::Mouse::GetRay().diff.z);
			ImGui::Text("Mouse Wheel: %d", Game::Input::Mouse::GetWheel());

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
			struct KeyInfo
			{
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

			for (const auto& k : kKeys)
			{
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

	Vector3 from0 = Vector3{ 1.0f,0.7f,0.5f }.Normalize();
	Vector3 to0 = -from0;
	Vector3 from1 = Vector3{ -0.6f,0.9f,0.2f }.Normalize();
	Vector3 to1 = Vector3{ 0.4f,0.7f,-0.5f }.Normalize();
	Matrix4x4 rotateMatrix0 = Matrix4x4::DirectionToDirectionMatrix(Vector3{ 1.0f,0.0f,0.0f }, Vector3{ -1.0f,0.0f,0.0f });
	Matrix4x4 rotateMatrix1 = Matrix4x4::DirectionToDirectionMatrix(from0, to0);
	Matrix4x4 rotateMatrix2 = Matrix4x4::DirectionToDirectionMatrix(from1, to1);

	ImGui::Begin("DirectionToDirectionMatrix Test");
	ImGui::Text("from0 : (%5.2f, %5.2f, %5.2f)", from0.x, from0.y, from0.z);
	ImGui::Text("to0   : (%5.2f, %5.2f, %5.2f)", to0.x, to0.y, to0.z);
	ImGui::Text("from1 : (%5.2f, %5.2f, %5.2f)", from1.x, from1.y, from1.z);
	ImGui::Text("to1   : (%5.2f, %5.2f, %5.2f)", to1.x, to1.y, to1.z);
	ImGui::Text("rotateMatrix0 :");
	ImGui::NewLine();
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			ImGui::SameLine();
			ImGui::Text("%6.3f ", rotateMatrix0.m[j][i]);
		}
		ImGui::NewLine();
	}
	ImGui::Text("rotateMatrix1 :");
	ImGui::NewLine();
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			ImGui::SameLine();
			ImGui::Text("%6.3f ", rotateMatrix1.m[j][i]);
		}
		ImGui::NewLine();
	}
	ImGui::Text("rotateMatrix2 :");
	ImGui::NewLine();
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			ImGui::SameLine();
			ImGui::Text("%6.3f ", rotateMatrix2.m[j][i]);
		}
		ImGui::NewLine();
	}
	ImGui::End();

	quaternion q0 = quaternion{ 2.0f,3.0f,4.0f,1.0f };
	quaternion q1 = quaternion{ 1.0f,3.0f,5.0f,2.0f };
	quaternion identity = quaternion::MakeIdentityQuaternion();
	quaternion conj = quaternion::MakeConjugateQuaternion(q0);
	quaternion inv = quaternion::Inverse(q0);
	quaternion normal = quaternion::Normalize(q0);
	quaternion mul1 = q0 * q1;
	quaternion mul2 = q1 * q0;
	float norm = quaternion::Norm(q0);
	ImGui::Begin("Quaternion Test");
	ImGui::Text("q1       : (%5.2f, %5.2f, %5.2f, %5.2f)", q0.x, q0.y, q0.z, q0.w);
	ImGui::Text("q2       : (%5.2f, %5.2f, %5.2f, %5.2f)", q1.x, q1.y, q1.z, q1.w);
	ImGui::Text("identity : (%5.2f, %5.2f, %5.2f, %5.2f)", identity.x, identity.y, identity.z, identity.w);
	ImGui::Text("conj     : (%5.2f, %5.2f, %5.2f, %5.2f)", conj.x, conj.y, conj.z, conj.w);
	ImGui::Text("inv      : (%5.2f, %5.2f, %5.2f, %5.2f)", inv.x, inv.y, inv.z, inv.w);
	ImGui::Text("normal   : (%5.2f, %5.2f, %5.2f, %5.2f)", normal.x, normal.y, normal.z, normal.w);
	ImGui::Text("mul1     : (%5.2f, %5.2f, %5.2f, %5.2f)", mul1.x, mul1.y, mul1.z, mul1.w);
	ImGui::Text("mul2     : (%5.2f, %5.2f, %5.2f, %5.2f)", mul2.x, mul2.y, mul2.z, mul2.w);
	ImGui::Text("norm     : %5.2f", norm);
	ImGui::End();

	ImGui::ShowDebugLogWindow();

	//📐 レイアウト・カーソル操作
	//	- NewLine() : 改行して次の行へ
	//	- SameLine() : 同じ行に次の要素を配置
	//	- Spacing() : 標準的な余白を挿入
	//	- Separator() : 水平線を描画
	//	- Dummy() : 指定サイズの空白を確保
	//📝 テキスト関連
	//	- Text() : テキスト表示
	//	- BulletText() : 箇条書き風テキスト
	//	- LabelText() : ラベル＋値の組み合わせ表示
	//🎛 基本ウィジェット
	//	- Button() : ボタン
	//	- Checkbox() : チェックボックス
	//	- RadioButton() : ラジオボタン
	//	- ProgressBar() : 進捗バー
	//	- Image() : 画像表示
	//🎚 スライダー・入力
	//	- SliderFloat() / SliderInt() : スライダーで数値入力
	//	- DragFloat() / DragInt() : ドラッグ操作で数値入力
	//	- InputText() : テキスト入力
	//	- InputFloat() / InputInt() : 数値入力
	//🎨 カラー関連
	//	- ColorEdit3() / ColorEdit4() : RGB / RGBA カラー編集
	//	- ColorPicker3() / ColorPicker4() : カラーピッカー
	//📂 コンボ・リスト
	//	- Combo() : ドロップダウン選択
	//	- ListBox() : リスト選択
	//	- Selectable() : 選択可能アイテム
	//🌳 階層構造
	//	- TreeNode() / TreePop() : ツリー表示
	//	- CollapsingHeader() : 折り畳み可能なヘッダー
	//🍴 メニュー・ポップアップ
	//	- BeginMenu() / EndMenu() : メニュー
	//	- OpenPopup() / BeginPopup() / EndPopup() : ポップアップ
	//	- BeginTooltip() / EndTooltip() : ツールチップ
	//📊 テーブル・カラム
	//	- BeginTable() / EndTable() : テーブル表示
	//	- TableNextRow() / TableNextColumn() : テーブル操作
	//	- レガシー API : Columns()
	//🔧 ユーティリティ
	//	- IsItemHovered() : マウスが乗っているか判定
	//	- IsItemActive() : アクティブ状態判定

}

