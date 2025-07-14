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

	// 描画
	static void Drawobj    (const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor);
	static void Drawobj    (const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSphere (const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawSphere (const Transforms& transform, const Vector3& center, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawSprite (const Transforms& transform, const Vector2& center, const Vector2& textureSize, uint32_t textureNumber, const uint32_t& materialColor, const Transforms& uvTransform);
	static void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor);
	static void DrawTriangle(const Transforms& transform, const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, uint32_t textureNumber, const uint32_t& materialColor, const DrawOptions drawOptions);
	static void DrawGrid	();
	
	static void DrawLine   (const Vector3& start, const Vector3& end, const uint32_t& materialColor);

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

};

