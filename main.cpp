#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s




int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");

	// 変数宣言
	int uvChecker = Game::LoadTexture("resources/uvChecker.png");
	int monsterBall = Game::LoadTexture("resources/monsterBall.png");
	int white1x1 = Game::LoadTexture("resources/white1x1.png");
	int obj1 = Game::LoadOBJ("resources", "axis.obj");
	int obj2 = Game::LoadOBJ("resources", "cone.obj");
	Transforms transformOBJ1;
	transformOBJ1.translate = { 1.0f,1.0f,1.0f };
	Transforms transformOBJ2;
	transformOBJ1.translate = { 2.0f,2.0f,2.0f };

	bool autoRotation[3] = { 0,0,0 };


	Vector4 color = { 1.0f, 1.0f, 0.0f, 1.0f };


	Transforms transformSphere1;
	transformSphere1.scale = { 0.5f, 0.5f, 0.5f };
	// 球の分割数
	const uint32_t kSubdivision = 16;
	// 頂点リソースにデータを書き込む
	VertexData vertexData[kSubdivision * kSubdivision * 6 ];


	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();



		///
		/// ↓更新処理ここから
		///
		transformOBJ2.rotate.x += 0.01f;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		Game::Drawobj(transformOBJ1, obj1, uvChecker, color);
		Game::Drawobj(transformOBJ2, obj2, monsterBall, color);


		Game::DrawSphere(transformSphere1, vertexData, kSubdivision, white1x1, color);


		const char* items[] = { "axis.obj", "plane.obj" };
		if (ImGui::CollapsingHeader("object", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("ObjectScale", &Game::GetTransforms(obj1)->scale.x, 0.01f);
			ImGui::DragFloat3("ObjectRotate", &Game::GetTransforms(obj1)->rotate.x, 0.01f);
			ImGui::DragFloat3("ObjectTranslate", &Game::GetTransforms(obj1)->translate.x, 0.01f);
			ImGui::SliderAngle("ObjectRotateX", &Game::GetTransforms(obj1)->rotate.x);
			ImGui::SliderAngle("ObjectRotateY", &Game::GetTransforms(obj1)->rotate.y);
			ImGui::SliderAngle("ObjectRotateZ", &Game::GetTransforms(obj1)->rotate.z);
			ImGui::Checkbox("X", &autoRotation[0]);
			ImGui::SameLine(0.0f, 54.0f);
			ImGui::Checkbox("Y", &autoRotation[1]);
			ImGui::SameLine(0.0f, 54.0f);
			ImGui::Checkbox("Z", &autoRotation[2]);
			ImGui::SameLine(0.0f, 54.0f);
			ImGui::Text("AutoRotation");


			ImGui::DragFloat3("transformOBJ1Scale", &transformOBJ1.scale.x, 0.01f);
			ImGui::DragFloat3("transformOBJ1Rotate", &transformOBJ1.rotate.x, 0.01f);
			ImGui::DragFloat3("transformOBJ1Translate", &transformOBJ1.translate.x, 0.01f);

			ImGui::DragFloat3("transformOBJ2Scale", &transformOBJ2.scale.x, 0.02f);
			ImGui::DragFloat3("transformOBJ2Rotate", &transformOBJ2.rotate.x, 0.02f);
			ImGui::DragFloat3("transformOBJ2Translate", &transformOBJ2.translate.x, 0.02f);
		}
		if (ImGui::CollapsingHeader("sphere"))
		{
			ImGui::DragFloat3("transformSphere1Scale", &transformSphere1.scale.x, 0.01f);
			ImGui::DragFloat3("transformSphere1Rotate", &transformSphere1.rotate.x, 0.01f);
			ImGui::DragFloat3("transformSphere1Translate", &transformSphere1.translate.x, 0.01f);
		}

		if (autoRotation[0])Game::GetTransforms(obj1)->rotate.x += 0.01f;
		if (autoRotation[1])Game::GetTransforms(obj1)->rotate.y += 0.01f;
		if (autoRotation[2])Game::GetTransforms(obj1)->rotate.z += 0.01f;
		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();

	return 0;
}
