#pragma once
#include "definition/definition.h"
#include <array>


// カメラは常にcenter_を見る
// ↳オービットの時はcenterを中心に、カメラが回転する。
//  FPSの時はカメラを中心に、centerが回転する
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
    void SetCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float target, int spendFrame, EaseType easetype);

	// シェイク
    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking() const;
    void StopShake();

    // 視錐台内にAABBがあるか
    bool InFrustum(const AABB& aabb);
	// 視錐台内にAABBがあるか 中心に近いほど1.0、遠いほど0.0を返す
	float InFrustum_Lod(const AABB& aabb);
    
	// 操作可能か設定
	void SetEnableControl(bool enable) { enableControl_ = enable; }

	void SetCameraMode(CameraMode_ORBIT_FPS mode) { cameraMode_ = mode; }

	// カメラ名
	std::string name_;


public:
	// 情報取得

	Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix; }

	Vector3 GetCenter() const { return center_; }

	Vector3 GetTranslate() const { return currentPosCartesian_; }

	Vector3 GetRotate() const { return Vector3(-currentPosSpherical_.theta, currentPosSpherical_.phi, 0.0f); }

	float GetDistance() const { return currentPosSpherical_.radius; }

private:
    Vector3 GetShakeOffset() const;

	void Update_Orbit();
	void Update_FPS();

private:

    CameraMode_ORBIT_FPS cameraMode_ = CameraMode_ORBIT_FPS::ORBIT;

    // 操作可能か
    bool enableControl_;

	// 球面座標上の現在位置
	Coordinate_spherical currentPosSpherical_;
	// デカルト座標上の現在位置
	Vector3 currentPosCartesian_;
	// カメラの回転中心
	Vector3 center_ = { 0.0f, 0.0f, 0.0f };

    //////////////////////////////////////////////
    ///              カメラ回転                ///
    //////////////////////////////////////////////
    void MovingCenter();


    //////////////////////////////////////////////
    ///                回転中心                ///
    //////////////////////////////////////////////
    void MovingRotate();


    //////////////////////////////////////////////
    ///               カメラ距離               ///
    //////////////////////////////////////////////
    void MovingDistance();


    //////////////////////////////////////////////
    ///             視錐台判定用              ///
    //////////////////////////////////////////////
    void CreateFrustumPlanes();
    std::array<Plane, 6> frustumPlanes_;// 視錐台を構成する6つの平面
	
    //////////////////////////////////////////////
    ///              カメラシェイク            ///
    //////////////////////////////////////////////

    Matrix4x4 viewportMatrix;

	// ビュー行列関連データ
    Matrix4x4 viewMatrix_;

    /// プロジェクション行列関連データ
    Matrix4x4 projectionMatrix_;
	float fovY_ = 0.45f;
    float aspect_;
    float nearZ_ = 0.01f;
	float farZ_ = 1000.0f;

	// ビュープロジェクション行列
    Matrix4x4 viewProjectionMatrix;
};