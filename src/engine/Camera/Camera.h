#pragma once
#include "definition/definition.h"
#include <array>

struct easingSet
{
    Vector3 start;
    Vector3 end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

class Camera
{
public:
    Camera();
    ~Camera();

    void Update();
    void Draw();
	void DrawImGui();
    void Resize();

	// 動かす先の設定
    void SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float Center, int spendFrame, EaseType easetype);

	// シェイク
    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking() const;
    void StopShake();
    Vector3 GetShakeOffset() const;

	// 情報取得
    Vector3 center_;
    float distance_;
    Transforms GetTransform() const { return transform_; }
	Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix; }

    // 視錐台内にAABBがあるか
    bool InFrustum(const AABB& aabb);

    // 操作可能か
    bool enableControl_;

	std::string name_;

private:

	bool orbitMode_ = false;
	void Updata_Orbit();
	void Update_FPS();

    void MovingCenter();
    void MovingRotate();
    void MovingDistance();

    //////////////////////////////////////////////
    ///              カメラ回転                ///
    //////////////////////////////////////////////
    Vector2 mousePosition_;
    Vector2 preMousePosition_;
    Vector2 mousePositionGap_;
    Vector3 preRotate_;
    // 演出による回転中
    easingSet easeRotate_;

    //////////////////////////////////////////////
    ///                回転中心                ///
    //////////////////////////////////////////////
    bool pressMouse2_ = 0;
    bool prePressMouse2_ = 0;
    Vector3 preCenter_;
    Vector3 normalize_;
    // 演出による回転中心の変更中
    easingSet easeCenter_;

    //////////////////////////////////////////////
    ///               カメラ距離               ///
    //////////////////////////////////////////////
    bool pressMouse0_ = 0;
    bool prePressMouse0_ = 0;
    int mouseWheel_ = 0;
    // 演出によるカメラ距離の変更中
    easingSet easeDistance_;

    //////////////////////////////////////////////
    ///             視錐台判定用              ///
    //////////////////////////////////////////////
    void CreateFrustumPlanes();
    std::array<Plane, 6> frustumPlanes_;// 視錐台を構成する6つの平面

    //////////////////////////////////////////////
    ///              カメラシェイク            ///
    //////////////////////////////////////////////
    bool shakeActive_ = false;
    float shakeDuration_ = 0.0f;		// 揺れが続く時間
    float shakeTime_ = 0.0f;			// 経過時間
    float shakeIntensity_ = 0.0f;		// 初期振幅（揺れの強さ）
    float shakeFrequency_ = 25.0f;		// 揺れる速さ

    Matrix4x4 viewportMatrix;

	// ビュー行列関連データ
    Matrix4x4 viewMatrix_;
    Transforms transform_;
    Matrix4x4 worldMatrix_;

    /// プロジェクション行列関連データ
    Matrix4x4 projectionMatrix_;
	float fovY_ = 0.45f;
    float aspect_;
    float nearZ_ = 0.1f;
	float farZ_ = 100.0f;

	// ビュープロジェクション行列
    Matrix4x4 viewProjectionMatrix;
};