#include "Game.h"           // ゲームロジック
#include "functions.h"      // 関数s

#define TRIANGLE_SUM 60


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウ、DrectX初期化
	Game::Initialize(1280, 720, L"CG2");

	int uvChecker = Game::LoadTexture("resources/uvChecker.png");
	int monsterBall = Game::LoadTexture("resources/monsterBall.png");
	int white1x1 = Game::LoadTexture("resources/white1x1.png");
	int obj2 = Game::LoadOBJ("resources", "cone.obj");
	int obj1 = Game::LoadOBJ("resources", "axis.obj");

	Transforms transformsObj;
	transformsObj.scale = { 1.2f, 1.2f, 1.2f };
	transformsObj.translate = { 0.0f,0.0f,0.0f };
	transformsObj.rotate = { 0.0f,(36.0f / 60.0f),1.5f };

	Vector4 color = { 1.0f,0.0f,0.0f,1.0f };

	Transforms transform = { {1,1,1}, {0,0,0}, {0,0,0} };
	Transforms transform1 = { {1,1,1}, {0,0,0}, {0,0,0} };
	VertexData vdata[4];
	// 右下 左下
	vdata[0].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vdata[0].texcoord = { 0.0f, 0.0f };
	vdata[0].normal = { 0.0f,0.0f,-1.0f };
	// 左下 左上
	vdata[1].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vdata[1].texcoord = { 1.0f, 0.0f };
	vdata[1].normal = { 0.0f,0.0f,-1.0f };
	// 右上 右下
	vdata[2].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vdata[2].texcoord = { 0.0f, 1.0f };
	vdata[2].normal = { 0.0f,0.0f,-1.0f };
	// 左上 右上
	vdata[3].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vdata[3].texcoord = { 1.0f, 1.0f };
	vdata[3].normal = { 0.0f,0.0f,-1.0f };

	// 頂点データ例
	VertexData triangleVertices[3] = {
		{ {0.0f, 0.5f, 0.0f, 1.0f}, {0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
		{ {0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
		{ {-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }
	};
	// ローカル変換
	Transforms localTransform = {};
	localTransform.scale = { 1.0f, 1.0f, 1.0f };
	localTransform.rotate = { 0.0f, 0.0f, 0.0f };
	localTransform.translate = { 0.0f, 0.0f, 0.0f };

	// ワールド変換
	Transforms worldTransform = {};
	worldTransform.scale = { 1.0f, 1.0f, 1.0f };
	worldTransform.rotate = { 0.0f, 0.0f, 0.0f };
	worldTransform.translate = { 0.0f, 0.0f, 0.0f };

	// テクスチャ番号（0番はデフォルトのテクスチャ）
	uint32_t textureNumber = 0;

	// マテリアルカラー（白）
	Vector4 materialColor = { 1.0f, 1.0f, 1.0f, 1.0f };


	Transforms transformSphere1;
	transformSphere1.scale = { 0.3f, 0.3f, 0.3f };
	transformSphere1.rotate = { 0.0f,4.72f,0.0f };
	// 球の分割数
	const uint32_t kSubdivision = 16;
	// 頂点リソースにデータを書き込む
	VertexData vertexData[kSubdivision * kSubdivision * 6];

	Vector2 mousePosition = { 0,0 };

	while (Game::ProcessMessage())
	{
		// フレームの開始
		Game::BeginFrame();



		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///
		
		///
		/// ↓描画処理ここから
		///

		Game::DrawTriangle(localTransform, worldTransform, triangleVertices, textureNumber, materialColor);

		Game::DrawSprite(transform, vdata, uvChecker, color);
		Game::DrawSprite(transform1, vdata, uvChecker, color);
		Game::Drawobj(transformsObj, transformsObj.translate, obj1, uvChecker, color);
		Game::DrawSphere(transformSphere1, vertexData, kSubdivision, monsterBall, { 1.0f, 1.0f, 1.0f, 1.0f });

		Game::GetMousePosition(&mousePosition);

		if (ImGui::CollapsingHeader("sphere"))
		{
			//ImGui::DragFloat3("SphereScale", &Game::cameraTransform.scale.x, 0.01f);
			//ImGui::DragFloat3("SphereRotate", &Game::cameraTransform.rotate.x, 0.01f);
			//ImGui::DragFloat3("SphereTranslate", &Game::cameraTransform.translate.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("Sprite"))
		{
			ImGui::DragFloat3("SpriteScale", &transform.scale.x, 0.01f);
			ImGui::DragFloat3("SpriteRotate", &transform.rotate.x, 0.01f);
			ImGui::DragFloat3("SpriteTranslate", &transform.translate.x, 0.1f);
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
