#pragma once
#include <vector>
#include "definition/definition.h"

class Camera;

class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	void Update();
	void Draw();
    void Resize();

    void SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float Center, int spendFrame, EaseType easetype);

    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking();
    void StopShake();

	Vector3 GetCenter() const;		// カメラ回転中心
	Vector3 GetTranslate() const;	// カメラ位置
	Matrix4x4 GetViewProjectionMatrix() const; // ビュープロジェクション行列
	float GetDistance() const;		// カメラ距離

	// 視錐台内にAABBがあるか
	bool InCamera(const AABB& aabb);

private:
	int currentCameraID_ = 0;

	std::vector<Camera> camera_;



};
