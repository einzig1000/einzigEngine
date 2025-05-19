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


	Transforms transformTriangle1;
	Transforms transformTriangle2;
	Transforms transformTriangle3;
	VertexData vertexDataTriangle1[3];
	VertexData vertexDataTriangle2[3];
	Vector4 triColor1 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 triColor2 = { 1.0f,1.0f,1.0f,1.0f };
	const char* items1[] = { "uvChecker", "monsterBall","white1x1" };
	const char* items2[] = { "uvChecker", "monsterBall","white1x1" };
	static int item_current1 = 0;
	static int item_current2 = 0;
	// 左下
	vertexDataTriangle1[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexDataTriangle1[0].texcoord = { 0.0f,1.0f };
	// 上
	vertexDataTriangle1[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	vertexDataTriangle1[1].texcoord = { 0.5f,0.0f };
	// 右下
	vertexDataTriangle1[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	vertexDataTriangle1[2].texcoord = { 1.0f,1.0f };

	Vector3 normal = CalculateNormal(vertexDataTriangle1[0].position, vertexDataTriangle1[1].position, vertexDataTriangle1[2].position);
	vertexDataTriangle1[0].normal = normal;
	vertexDataTriangle1[1].normal = normal;
	vertexDataTriangle1[2].normal = normal;


	// 左下2
	vertexDataTriangle2[0].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexDataTriangle2[0].texcoord = { 0.0f,1.0f };
	// 上2
	vertexDataTriangle2[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataTriangle2[1].texcoord = { 0.5f,0.0f };
	// 右下2
	vertexDataTriangle2[2].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexDataTriangle2[2].texcoord = { 1.0f,1.0f };

	normal = CalculateNormal(vertexDataTriangle2[0].position, vertexDataTriangle2[1].position, vertexDataTriangle2[2].position);
	vertexDataTriangle2[0].normal = normal;
	vertexDataTriangle2[1].normal = normal;
	vertexDataTriangle2[2].normal = normal;





	bool autoRotation2[3] = { 0,0,0 };
	bool autoRotation[3] = { 0,0,0 };
	bool translate[3] = { 0,0,0 };


	Transforms transformSphere1;
	transformSphere1.scale = { 0.3f, 0.3f, 0.3f };
	transformSphere1.rotate = { 0.0f,4.72f,0.0f };
	// 球の分割数
	const uint32_t kSubdivision = 16;
	// 頂点リソースにデータを書き込む
	VertexData vertexData[kSubdivision * kSubdivision * 6];


	float speed = 1.0f;


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
				transformsObjworld[i].rotate.y += 0.1f * speed;
				transformsObj[i].translate.y += 0.004f * speed;
				transformsObj[i].translate.z += 0.004f * speed;
				if (transformsObj[i].translate.y > 4)
				{
					transformsObjworld[i].rotate.y = 0.0f;
					transformsObj[i].translate.y = 0.0f;
					transformsObj[i].translate.z = 0.0f;
					flag[i] = 0;
				}
				else if (transformsObj[i].translate.y < 0)
				{
					transformsObjworld[i].rotate.y = 100.0f;
					transformsObj[i].translate.y = 4.0f;
					transformsObj[i].translate.z = 4.0f;
					flag[i] = 0;
				}
			}
		}



		///
		/// ↑更新処理ここまで
		///
		frame++;
		if (frame % int(20 / sqrtf(speed)) == 0)
		{
			flag[frame / int(20 / sqrtf(speed))] = 1;
		}
		if (frame > int(20 / sqrtf(speed)) * TRIANGLE_SUM)
		{
			frame = 0;
		}
		///
		/// ↓描画処理ここから
		///

		for (int i = 0; i < TRIANGLE_SUM; ++i)
		{
			Game::Drawobj(transformsObj[i], transformsObjworld[i],obj1, monsterBall, color[i]);

		}

		Game::DrawSphere(transformSphere1, vertexData, kSubdivision, monsterBall, { 1.0f, 1.0f, 1.0f, 1.0f });



		if (item_current1 == 0)Game::DrawTriangle(transformTriangle1, transformTriangle3, vertexDataTriangle1, 3, uvChecker, triColor1);
		if (item_current1 == 1)Game::DrawTriangle(transformTriangle1, transformTriangle3, vertexDataTriangle1, 3, monsterBall, triColor1);
		if (item_current1 == 2)Game::DrawTriangle(transformTriangle1, transformTriangle3, vertexDataTriangle1, 3, white1x1, triColor1);

		if (item_current2 == 0)Game::DrawTriangle(transformTriangle2, transformTriangle3, vertexDataTriangle2, 3, uvChecker, triColor2);
		if (item_current2 == 1)Game::DrawTriangle(transformTriangle2, transformTriangle3, vertexDataTriangle2, 3, monsterBall, triColor2);
		if (item_current2 == 2)Game::DrawTriangle(transformTriangle2, transformTriangle3, vertexDataTriangle2, 3, white1x1, triColor2);





		if (ImGui::CollapsingHeader("object", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("objectSpeed", &speed, 0.01f);

		}
		if (ImGui::CollapsingHeader("triangle", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("triangle1 & triangle2");
			ImGui::DragFloat3("Triangle1&2Scale", &transformTriangle3.scale.x, 0.01f);
			ImGui::DragFloat3("Triangle1&2Rotate", &transformTriangle3.rotate.x, 0.01f);
			ImGui::DragFloat3("Triangle1&2Translate", &transformTriangle3.translate.x, 0.01f);

			ImGui::Text("triangle1");
			ImGui::Combo("Triangle1Texture", &item_current1, items1, IM_ARRAYSIZE(items1));
			ImGui::DragFloat3("Triangle1Scale", &transformTriangle1.scale.x, 0.01f);
			ImGui::DragFloat3("Triangle1Rotate", &transformTriangle1.rotate.x, 0.01f);
			ImGui::DragFloat3("Triangle1Translate", &transformTriangle1.translate.x, 0.01f);
			ImGui::ColorEdit3("Triangle1Color", (float*)&triColor1.x);

			ImGui::Text("triangle2");
			ImGui::Combo("Triangle2Texture", &item_current2, items2, IM_ARRAYSIZE(items2));
			ImGui::DragFloat3("Triangle2Scale", &transformTriangle2.scale.x, 0.01f);
			ImGui::DragFloat3("Triangle2Rotate", &transformTriangle2.rotate.x, 0.01f);
			ImGui::DragFloat3("Triangle2Translate", &transformTriangle2.translate.x, 0.01f);
			ImGui::ColorEdit3("Triangle2Color", (float*)&triColor2.x);
		}
		if (ImGui::CollapsingHeader("sphere"))
		{
			ImGui::DragFloat3("SphereScale", &transformSphere1.scale.x, 0.01f);
			ImGui::DragFloat3("SphereRotate", &transformSphere1.rotate.x, 0.01f);
			ImGui::DragFloat3("SphereTranslate", &transformSphere1.translate.x, 0.01f);
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
