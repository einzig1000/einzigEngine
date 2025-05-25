#pragma once
#include "definition.h"

class CameraController
{
public:

	CameraController();
	void Updata();
	void Draw();

    static Matrix4x4 viewportMatrix;
    static Matrix4x4 viewProjectionMatrix;

    bool cameraMode;

private:

    //////////////////////////////////////////////
    ///              カメラ回転                ///
    //////////////////////////////////////////////
    Vector2 mousePosition;
    Vector2 preMousePosition;
    Vector2 mousePositionGap;
    Vector2 preRotate;

    //////////////////////////////////////////////
    ///                回転中心                ///
    //////////////////////////////////////////////
    // カメラがどこを中心に回転するか（現在は原点中心）
    Vector3 target = { 0.0f, 0.0f, 0.0f };
    bool pressMouse2 = 0;
    bool prePressMouse2 = 0;
    Vector3 preTarget;
    Vector3 normalize;
    //Sphere sphere;
    //TransformationMatrix sphereTransforms;

    //////////////////////////////////////////////
    ///               カメラ距離               ///
    //////////////////////////////////////////////
    // カメラの距離（cameraTargetからの距離）
    float distance = 15.00f; // 6.49f
    bool pressMouse0 = 0;
    bool prePressMouse0 = 0;
    int mouseWheel = 0;


    //////////////////////////////////////////////
    ///               カメラ移動               ///
    ////////////////////////////////////////////// 
public:
    Matrix4x4 cameraMatrix;
    Transforms transform;
    Matrix4x4 projectionMatrix;
    Matrix4x4 viewMatrix;

};

