#pragma once
#include "definition/definition.h"

struct easingSet
{
    Vector3 start;
    Vector3 end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

class CameraController
{
public:

    CameraController();
    void Update();
    void Draw();

    void MovingCenter();
    void MovingRotate();
    void MovingDistance();

    void SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float Center, int spendFrame, EaseType easetype);

    Vector3 GetCenter() const { return center_; }

    bool cameraMode_;
    bool cameraMode_centerControl_;
    bool cameraMode_rotateControl_;
    bool cameraMode_distanceControl_;
    bool cameraModeMode_;

private:
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
    // カメラがどこを中心に回転するか（現在は原点中心）
    Vector3 center_;
    bool pressMouse2_ = 0;
    bool prePressMouse2_ = 0;
    Vector3 preCenter_;
    Vector3 normalize_;
    // 演出による回転中心の変更中
    easingSet easeCenter_;
    DrawOptions sphereOptions;

    //////////////////////////////////////////////
    ///               カメラ距離               ///
    //////////////////////////////////////////////
    // カメラの距離（cameraCenterからの距離）
    float distance_; // 6.49f
    bool pressMouse0_ = 0;
    bool prePressMouse0_ = 0;
    int mouseWheel_ = 0;
    // 演出によるカメラ距離の変更中
    easingSet easeDistance_;


    //////////////////////////////////////////////
    ///               カメラ移動               ///
    ////////////////////////////////////////////// 
public:
    Matrix4x4 viewportMatrix;
    Matrix4x4 viewProjectionMatrix;
    Matrix4x4 cameraMatrix_;
    Transforms transform_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 viewMatrix_;
};

