#pragma once
#include "Engine/Engine.h"

class Game
{
public:
	// 初期化
	static void Initialize(int width, int height, const std::wstring& title);

	// メインループ用
	static bool ProcessMessage();
	static void BeginFrame();
	static void EndFrame();

	// 終了処理
	static void Finalize();

	// リソース読み込み
	static uint32_t LoadOBJ(const std::string& directoryPath, const std::string& filename);
	static uint32_t LoadTexture(const std::string& filePath);
	static uint32_t LoadAudio(const std::string& filePath);
	TextureData* GetTexture(uint32_t textureNumber);

	// 描画
	static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor);
	static void Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawSphere(const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSprite(const Transforms& transform, const Vector2& center, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);


	static void DrawLine(const Vector3& start, const Vector3& end, const uint32_t& materialColor);

	// 音
	static void PlayAudio(const uint32_t& audioId, bool loop);
	static void StopAudio(const uint32_t& audioId);
	static void SetAudioVolume(const uint32_t& audioId, float volume);
	static void SetMasterVolume(float volume);
	static float GetVolume(const uint32_t& audioId);
	static float GetMasterVolume();
	static bool IsAudioPlaying(const uint32_t& audioId);

	// マウス
	static void GetMousePosition(Vector2* position);
	static void SetMouseRay();
	static bool IsCollisionMouseRayAABB(AABB aabb, int objNum);
	// 0 = 左クリック  1 = 右クリック  2 = ミドルボタン
	static bool IsPressMouse(int i);
	static uint32_t GetWheel();

	// カメラ
	static void MoveCenterTarget(Vector3 target, int spendFrame);
	static void MoveRotateTarget(Vector3 target, int spendFrame);
	static void MoveDistanceTarget(float target, int spendFrame);

	// AABBの作成
	static AABB CreateAABB(const Transforms& transforms, uint32_t objectNumber);

	// プリミティブモードの設定
	static void toggleWireframeMode();


	class RenderDate_Model
	{
	public:
		// 位置、回転、スケール
		Transforms transforms;
		// 親のワールドマトリックス
		TransformationMatrix parentTransformationMatrix;
		// 回転の中心点
		Vector3 pivot;
		// 色
		uint32_t color = 0xFFFFFFFF;
		// 3Dモデル
		uint32_t model = 0;
		// テクスチャ
		uint32_t texture = 0;
		// 描画オプション
		DrawOptions options;
		// 衝突判定用AABB
		AABB AABB;

		void Draw()
		{
			// 1. オブジェクトのスケール行列
			Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(this->transforms.scale);

			// 2. ワールド空間での最終的な位置への移動行列
			Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(this->transforms.translate);

			// 3. 回転の中心への移動 (centerを原点に移動)
			Matrix4x4 toRotationCenter = Matrix4x4::MakeTranslateMatrix(-this->pivot);

			// 4. 回転行列 (transform.rotate を center を中心とする回転として使う)
			Matrix4x4 rotateXMatrix = Matrix4x4::MakeRotateXMatrix(this->transforms.rotate.x);
			Matrix4x4 rotateYMatrix = Matrix4x4::MakeRotateYMatrix(this->transforms.rotate.y);
			Matrix4x4 rotateZMatrix = Matrix4x4::MakeRotateZMatrix(this->transforms.rotate.z);
			Matrix4x4 rotationMatrix = rotateZMatrix * rotateXMatrix * rotateYMatrix;

			// 5. 回転後、元の回転中心の位置に戻す
			Matrix4x4 fromRotationCenter = Matrix4x4::MakeTranslateMatrix(this->pivot);

			// 最終的なワールド行列の構築
			this->transforms.World =
				scaleMatrix *		 // 1. 拡縮はどうでもいい
				toRotationCenter *	 // 2. 回転中心を原点に移動
				rotationMatrix *	 // 3. 原点で回転 (centerを中心とした回転)
				fromRotationCenter * // 4. 回転したものを元の回転中心に戻す
				translateMatrix;	 // 5. 最終的なワールド位置へ移動


			Game::Drawobj(this->transforms, this->pivot, this->model, this->texture, this->color, this->options);
		}
	};

	class RenderDate_Sprite
	{
	public:
		// 位置、回転、スケール
		Transforms transforms;
		// 親のワールドマトリックス
		TransformationMatrix parentTransformationMatrix;
		// 回転の中心点
		Vector2 pivot = { 0,0 };
		// 色
		uint32_t color = 0xFFFFFFFF;
		// テクスチャ
		uint32_t texture = 0;
		// 描画オプション
		DrawOptions options;

		void Draw() const
		{
			Game::DrawSprite(this->transforms, this->pivot, this->texture, this->color, this->options);
		}
	};
};

