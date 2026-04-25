#pragma once
#include "definition/definition.h"
#include <array>

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
    Transforms GetTransform() const { return transform_; }
	Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix; }

    // 視錐台内にAABBがあるか
    bool InFrustum(const AABB& aabb);
	// 視錐台内にAABBがあるか 中心に近いほど1.0、遠いほど0.0を返す
	float InFrustum_Lod(const AABB& aabb);
    
	// 操作可能か設定
	void SetEnableControl(bool enable) { enableControl_ = enable; }

	void SetCameraMode(CameraMode_ORBIT_FPS mode) { cameraMode_ = mode; }

	// カメラ名
	std::string name_;

private:
    CameraMode_ORBIT_FPS cameraMode_ = CameraMode_ORBIT_FPS::ORBIT;

    // 操作可能か
    bool enableControl_;

	void Update_Orbit();
	void Update_FPS();

    void MovingCenter();
    void MovingRotate();
    void MovingDistance();

	// 球面座標上の現在位置
	Coordinate_spherical currentPosSpherical_;
	// デカルト座標上の現在位置
	Vector3 currentPosCartesian_;
	// カメラの回転中心
	Vector3 center_ = { 0.0f, 0.0f, 0.0f };

    //////////////////////////////////////////////
    ///              カメラ回転                ///
    //////////////////////////////////////////////


    //////////////////////////////////////////////
    ///                回転中心                ///
    //////////////////////////////////////////////


    //////////////////////////////////////////////
    ///               カメラ距離               ///
    //////////////////////////////////////////////


    //////////////////////////////////////////////
    ///             視錐台判定用              ///
    //////////////////////////////////////////////
    void CreateFrustumPlanes();
    std::array<Plane, 6> frustumPlanes_;// 視錐台を構成する6つの平面
	std::array<Plane, 6> centerFrustumPlanes_;// 画面中心に入っているか確認するための狭めた視錐台

    //////////////////////////////////////////////
    ///              カメラシェイク            ///
    //////////////////////////////////////////////

    Matrix4x4 viewportMatrix;

	// ビュー行列関連データ
    Matrix4x4 viewMatrix_;
    Transforms transform_;
    //Matrix4x4 worldMatrix_;

    /// プロジェクション行列関連データ
    Matrix4x4 projectionMatrix_;
	float fovY_ = 0.45f;
    float aspect_;
    float nearZ_ = 0.01f;
	float farZ_ = 1000.0f;

	// ビュープロジェクション行列
    Matrix4x4 viewProjectionMatrix;
};