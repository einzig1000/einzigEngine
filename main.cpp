#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s

#define TRIANGLE_SUM 60


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");

	// 変数宣言
	int uvChecker = Game::LoadTexture("resources/uvChecker.png");
	int monsterBall = Game::LoadTexture("resources/monsterBall.png");
	int white1x1 = Game::LoadTexture("resources/white1x1.png");
	int obj1 = Game::LoadOBJ("resources", "cone.obj");
	int obj2 = Game::LoadOBJ("resources", "axis.obj");



	Transforms transformsObj[TRIANGLE_SUM];
	Transforms transformsObjworld[TRIANGLE_SUM];
	Vector4 color[TRIANGLE_SUM];
	bool flag[TRIANGLE_SUM];
	for (int i = 0; i < TRIANGLE_SUM; ++i)
	{
		transformsObj[i].scale = { 0.2f, 0.2f, 0.2f };
		transformsObj[i].translate = { 0.0f,0.0f,0.0f };
		transformsObj[i].rotate = { 0.0f,(36.0f / 60.0f) * i,1.5f };
		color[i] = { 1.0f, 1.0f, 1.0f, 1.0f };
		flag[i] = 0;
	}


	//Transforms transformsObj[3][TRIGROUP1];
	//Transforms transformsObjworld[3][TRIGROUP1];
	//Vector4 color[3][TRIGROUP1];
	//for (int i = 0; i < TRIGROUP1; ++i)
	//{
	//	transformsObj[0][i].scale = { 0.2f, 0.2f, 0.2f };
	//	transformsObj[0][i].translate = { 0.0f,0.0f,0.0f };
	//	transformsObj[0][i].rotate = { 0.0f,(36.0f / 60.0f) * i,1.5f };
	//	//transformsObjworld[0][i].rotate = { 0.0f, 0.0f, 0.0f };
	//	color[0][i] = { 0.7f, 0.2f, 0.3f, 0.2f };
	//}
	//for (int i = 0; i < TRIGROUP2; ++i)
	//{
	//	transformsObj[1][i].scale = { 0.2f, 0.2f, 0.2f };
	//	transformsObj[1][i].translate = { 0.0f,0.0f,0.0f };
	//	transformsObj[1][i].rotate = { 0.0f,(36.0f / 60.0f) * i,1.5f };
	//	//transformsObjworld[1][i].rotate = { 1.0f, 0.0f, 0.0f };
	//	color[1][i] = { 0.3f, 0.7f, 0.2f, 0.2f };
	//}
	//for (int i = 0; i < TRIGROUP3; ++i)
	//{
	//	transformsObj[2][i].scale = { 0.2f, 0.2f, 0.2f };
	//	transformsObj[2][i].translate = { 0.0f,0.0f,0.0f };
	//	transformsObj[2][i].rotate = { 0.0f,(36.0f / 60.0f) * i,1.5f };
	//	//transformsObjworld[2][i].rotate = { 2.0f, 0.0f, 0.0f };
	//	color[2][i] = { 0.2f, 0.3f, 0.7f, 0.2f };
	//}

	bool autoRotation2[3] = { 0,0,0 };
	bool autoRotation[3] = { 0,0,0 };


	bool translate[3] = { 0,0,0 };


	Transforms transformSphere1;
	transformSphere1.scale = { 0.3f, 0.3f, 0.3f };
	transformSphere1.rotate = { 0.0f,4.72f,0.0f };
	// 球の分割数
	const uint32_t kSubdivision = 16;
	// 頂点リソースにデータを書き込む
	VertexData vertexData[kSubdivision * kSubdivision * 6 ];

	int frame = 0;
	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();



		///
		/// ↓更新処理ここから
		///
		for (int i = 0; i < TRIANGLE_SUM; ++i)
		{
			if (flag[i])
			{
				transformsObjworld[i].rotate.y += 0.1f;
				transformsObj[i].translate.y += 0.004f;
				transformsObj[i].translate.z += 0.004f;
				if (transformsObj[i].translate.y > 3)
				{
					transformsObjworld[i].rotate.y = 0.0f;
					transformsObj[i].translate.y = 0.0f;
					transformsObj[i].translate.z = 0.0f;
					flag[i] = 0;
				}
			}
		}



		///
		/// ↑更新処理ここまで
		///
		frame++;
		if (frame % 20 == 0)
		{
			flag[frame / 20] = 1;
		}
		if (frame > 20 * TRIANGLE_SUM)
		{
			frame = 0;
		}
		///
		/// ↓描画処理ここから
		///

		for (int i = 0; i < TRIANGLE_SUM; ++i)
		{
			Game::Drawobj(
				{
				{ transformsObj[i].scale.x, transformsObj[i].scale.y, transformsObj[i].scale.z},
				{ transformsObj[i].rotate.x,transformsObj[i].rotate.y, transformsObj[i].rotate.z },
				{ transformsObj[i].translate.x, transformsObj[i].translate.y, transformsObj[i].translate.z} },

				{ transformsObjworld[i].scale,transformsObjworld[i].rotate,transformsObjworld[i].translate },
				obj1, white1x1, color[i]);

		}

		Game::DrawSphere(transformSphere1, vertexData, kSubdivision, monsterBall, { 1.0f, 1.0f, 1.0f, 1.0f });


		const char* items[] = { "axis.obj", "plane.obj" };
		if (ImGui::CollapsingHeader("object", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("ObjectScale", &Game::GetTransforms(obj1)->scale.x, 0.01f);
			ImGui::DragFloat3("ObjectRotate", &Game::GetTransforms(obj1)->rotate.x, 0.01f);
			ImGui::DragFloat3("ObjectTranslate", &Game::GetTransforms(obj1)->translate.x, 0.01f);
			ImGui::SliderAngle("ObjectRotateX", &Game::GetTransforms(obj1)->rotate.x);
			ImGui::SliderAngle("ObjectRotateY", &Game::GetTransforms(obj1)->rotate.y);
			ImGui::SliderAngle("ObjectRotateZ", &Game::GetTransforms(obj1)->rotate.z);

			ImGui::Text("1");
			ImGui::DragFloat3("transformsObjScale1", &transformsObj[0].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjRotate1", &transformsObj[0].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjTranslate1", &transformsObj[0].translate.x, 0.01f);
			ImGui::Text("2");
			ImGui::DragFloat3("transformsObjScale2", &transformsObj[1].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjRotate2", &transformsObj[1].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjTranslate2", &transformsObj[1].translate.x, 0.01f);
			ImGui::Text("3");
			ImGui::DragFloat3("transformsObjScale3", &transformsObj[2].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjRotate3", &transformsObj[2].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjTranslate3", &transformsObj[2].translate.x, 0.01f);

			ImGui::Text("1");
			ImGui::DragFloat3("transformsObjworldScale1", &transformsObjworld[0].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldRotate1", &transformsObjworld[0].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldTranslate1", &transformsObjworld[0].translate.x, 0.01f);
			ImGui::Text("2");
			ImGui::DragFloat3("transformsObjworldScale2", &transformsObjworld[1].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldRotate2", &transformsObjworld[1].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldTranslate2", &transformsObjworld[1].translate.x, 0.01f);
			ImGui::Text("3");
			ImGui::DragFloat3("transformsObjworldScale3", &transformsObjworld[2].scale.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldRotate3", &transformsObjworld[2].rotate.x, 0.01f);
			ImGui::DragFloat3("transformsObjworldTranslate3", &transformsObjworld[2].translate.x, 0.01f);



			ImGui::Checkbox("X1", &autoRotation[0]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("Y1", &autoRotation[1]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("Z1", &autoRotation[2]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Text("AutoRotation");
			ImGui::Checkbox("X2", &autoRotation2[0]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("Y2", &autoRotation2[1]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("Z2", &autoRotation2[2]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Text("AutoRotation2");
			ImGui::Checkbox("g1", &translate[0]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("g2", &translate[1]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Checkbox("g3", &translate[2]);
			ImGui::SameLine(0.0f, 51.0f);
			ImGui::Text("translate");


			//ImGui::DragFloat3("transformOBJ1Scale", &transformOBJ1.scale.x, 0.01f);
			//ImGui::DragFloat3("transformOBJ1Rotate", &transformOBJ1.rotate.x, 0.01f);
			//ImGui::DragFloat3("transformOBJ1Translate", &transformOBJ1.translate.x, 0.01f);

			//ImGui::DragFloat3("transformOBJ2Scale", &transformOBJ2.scale.x, 0.02f);
			//ImGui::DragFloat3("transformOBJ2Rotate", &transformOBJ2.rotate.x, 0.02f);
			//ImGui::DragFloat3("transformOBJ2Translate", &transformOBJ2.translate.x, 0.02f);
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

		if (translate[0])transformsObj[0].translate.y += 0.05f;
		if (translate[1])transformsObj[1].translate.y += 0.05f;
		if (translate[2])transformsObj[2].translate.y += 0.05f;

		//if (autoRotation2[0])
		//{
		//	for (int i = 0; i < TRIANGLE_SUM * 3; ++i)
		//	{
		//		transformsObjworld[i].rotate.x += 0.6f;
		//	}
		//}
		//if (autoRotation2[1])
		//{
		//	for (int i = 0; i < TRIANGLE_SUM * 3; ++i)
		//	{
		//		transformsObjworld[i].rotate.y += 0.2f;
		//	}
		//}
		//if (autoRotation2[2])
		//{
		//	for (int i = 0; i < TRIANGLE_SUM * 3; ++i)
		//	{
		//		transformsObjworld[i].rotate.z += 0.1f;
		//	}
		//}
		///
		/// ↑描画処理ここまで
		///




		// フレームの終了
		Game::EndFrame();
	}

	Game::Finalize();

	return 0;
}
