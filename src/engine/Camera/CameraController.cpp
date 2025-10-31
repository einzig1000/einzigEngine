#include "Camera/CameraController.h"
#include "Facade/Game.h"
#include "Window/WindowManager.h"

CameraController::CameraController()
{
    mousePositionGap_ = { 0,0 };
    enableControl_ = true;

    // カメラ
    transform_.translate = { 0.0f, 0.0f, 0.0f };
    transform_.rotate = { 1.13f, 0.0f, 0.0f };
    center_ = { 0.0f, 0.0f, 0.0f };
    distance_ = 35.60f;

    preCenter_ = center_;
    preRotate_.x = transform_.rotate.x;
    preRotate_.y = transform_.rotate.y;

    sphereOptions.enableLighting = false;

    Resize();
}

void CameraController::Update()
{
#pragma region カメラシェイク

    if (shakeActive_)
    {
        shakeTime_++;
     
        if (shakeTime_ >= shakeDuration_)
        {
            shakeActive_ = false;
            shakeTime_ = 0.0f;
        }
    }

#pragma endregion

#pragma region カメラ手動操作

    if (enableControl_)
    {
        // 左クリック
        prePressMouse0_ = pressMouse0_;
        pressMouse0_ = Game::Input::Mouse::GetMousePress(0);
        // ミドルボタン
        prePressMouse2_ = pressMouse2_;
        pressMouse2_ = Game::Input::Mouse::GetMousePress(2);

        mouseWheel_ = Game::Input::Mouse::GetMouseWheel();

#pragma region カメラ回転
        // クリックした瞬間
        if (pressMouse2_ && prePressMouse2_ == 0 && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            preMousePosition_ = Game::Input::Mouse::GetMousePosition();
        }
        // クリックしている最中
        if (pressMouse2_ && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            mousePosition_ = Game::Input::Mouse::GetMousePosition();
            mousePositionGap_.x = mousePosition_.x - preMousePosition_.x;
            mousePositionGap_.y = mousePosition_.y - preMousePosition_.y;
            transform_.rotate.x = (mousePositionGap_.y / 100.0f) + (preRotate_.x);
            transform_.rotate.y = (mousePositionGap_.x / 100.0f) + (preRotate_.y);
        }
        // クリックやめた瞬間
        if (prePressMouse2_ && pressMouse2_ == 0 && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            preRotate_ = transform_.rotate;
        }

#pragma endregion

#pragma region カメラ回転
        // クリックした瞬間
        if (pressMouse2_ && prePressMouse2_ == 0 && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            preMousePosition_ = Game::Input::Mouse::GetMousePosition();
        }
        // クリックしている最中
        if (pressMouse2_ && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            mousePosition_ = Game::Input::Mouse::GetMousePosition();
            mousePositionGap_.x = mousePosition_.x - preMousePosition_.x;
            mousePositionGap_.y = mousePosition_.y - preMousePosition_.y;
            transform_.rotate.x = (mousePositionGap_.y / 100.0f) + (preRotate_.x);
            transform_.rotate.y = (mousePositionGap_.x / 100.0f) + (preRotate_.y);
        }
        // クリックやめた瞬間
        if (prePressMouse2_ && pressMouse2_ == 0 && !GetHitKey::IsPressedNow(DIK_LSHIFT))
        {
            preRotate_ = transform_.rotate;
        }

#pragma endregion

#pragma region カメラ距離
        if (mouseWheel_ > 0)
        {
            distance_ -= float(mouseWheel_) / 100;
        }
        if (mouseWheel_ < 0)
        {
            distance_ -= float(mouseWheel_) / 100;
        }

#pragma endregion
    }

#pragma endregion

#pragma region カメラ演出処理

    if (easeRotate_.easingFlag)
    {
        MovingRotate();
    }
    if (easeCenter_.easingFlag)
    {
        MovingCenter();
    }
    if (easeDistance_.easingFlag)
    {
        MovingDistance();
    }

#pragma endregion

#pragma region カメラ行列計算


    // カメラ初期値
    Vector3 cameraLocalPos = { 0.0f, 0.0f, -distance_ };

    // カメラに回転適用
    Matrix4x4 cameraRotateMatrix = Matrix4x4::MakeAffineMatrix(
        { 1,1,1 },
        transform_.rotate,
        { 0,0,0 }
    );

    // cameraLocalPos　に　回転適用したマトリックスを適用させる　＝　原点上で回転
    Vector3 rotatedCameraPos = {
        cameraLocalPos.x * cameraRotateMatrix.m[0][0] + cameraLocalPos.y * cameraRotateMatrix.m[1][0] + cameraLocalPos.z * cameraRotateMatrix.m[2][0],
        cameraLocalPos.x * cameraRotateMatrix.m[0][1] + cameraLocalPos.y * cameraRotateMatrix.m[1][1] + cameraLocalPos.z * cameraRotateMatrix.m[2][1],
        cameraLocalPos.x * cameraRotateMatrix.m[0][2] + cameraLocalPos.y * cameraRotateMatrix.m[1][2] + cameraLocalPos.z * cameraRotateMatrix.m[2][2]
    };

    // 原点上で回転したrotatedCameraPosに　cameraCenterを足せば中心がcameraCenterに変わる
    transform_.translate = (center_ + rotatedCameraPos + GetShakeOffset());

    // カメラ行列を作成
    cameraMatrix_ = Matrix4x4::MakeAffineMatrix(
        { 1,1,1 },
        transform_.rotate,
        transform_.translate
    );

    // ビュー・射影・ビューポート行列
    viewMatrix_ = (cameraMatrix_.Inverse());
    viewProjectionMatrix = (viewMatrix_ * projectionMatrix_);

    CreateFrustumPlanes();
#pragma endregion

}

void CameraController::Resize()
{
    projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(WindowManager::winWidth_) / float(WindowManager::winHeight_), 0.1f, 100.0f);
    //viewportMatrix = Matrix4x4::MakeViewPortMatrix(0.0f, 0.0f, float(WindowManager::winWidth_), float(WindowManager::winHeight_), 0.0f, 1.0f);
}

void CameraController::Draw(bool debugCamera)
{
    ImGui::Begin("camera");
    if (debugCamera)ImGui::Text("Mode: Debug");
    else ImGui::Text("Mode: Release");
    ImGui::DragFloat3("cameraCenter", &center_.x, 0.01f);
    ImGui::DragFloat3("cameraRotate", &transform_.rotate.x, 0.01f);
    ImGui::DragFloat("cameraDistance", &distance_, 0.1f);
    ImGui::Checkbox("enableControl", &enableControl_);
	//if (debugCamera)ImGui::Checkbox("FixReleaseCamera", &enableControl_);
    ImGui::End();
	Game::DebugDraw::AddSphere(center_, Vector3{ 0.2f,0.2f,0.2f }, 0xFFFF00FF);
}

void CameraController::CreateFrustumPlanes()
{
    // Left Plane
    frustumPlanes_[0].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][0];
    frustumPlanes_[0].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][0];
    frustumPlanes_[0].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][0];
    frustumPlanes_[0].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][0];
    // Right Plane
    frustumPlanes_[1].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][0];
    frustumPlanes_[1].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][0];
    frustumPlanes_[1].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][0];
    frustumPlanes_[1].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][0];
    // Bottom Plane
    frustumPlanes_[2].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][1];
    frustumPlanes_[2].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][1];
    frustumPlanes_[2].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][1];
    frustumPlanes_[2].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][1];
    // Top Plane
    frustumPlanes_[3].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][1];
    frustumPlanes_[3].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][1];
    frustumPlanes_[3].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][1];
    frustumPlanes_[3].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][1];
    // Near Plane
    frustumPlanes_[4].normal.x = viewProjectionMatrix.m[0][2];
    frustumPlanes_[4].normal.y = viewProjectionMatrix.m[1][2];
    frustumPlanes_[4].normal.z = viewProjectionMatrix.m[2][2];
    frustumPlanes_[4].distance = viewProjectionMatrix.m[3][2];
    // Far Plane
    frustumPlanes_[5].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][2];
    frustumPlanes_[5].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][2];
    frustumPlanes_[5].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][2];
    frustumPlanes_[5].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][2];

    // 各平面を正規化
    for (int i = 0; i < 6; ++i)
    {
        float length = sqrt(frustumPlanes_[i].normal.x * frustumPlanes_[i].normal.x +
            frustumPlanes_[i].normal.y * frustumPlanes_[i].normal.y +
            frustumPlanes_[i].normal.z * frustumPlanes_[i].normal.z);
        frustumPlanes_[i].normal = frustumPlanes_[i].normal / length;
        frustumPlanes_[i].distance /= length;
    }
}

bool CameraController::InFrustum(const AABB& aabb)
{
    // AABBの8つの頂点をワールド空間に変換
    Vector3 points[8];

    points[0] = Vector3{ aabb.min.x, aabb.min.y, aabb.min.z };
    points[1] = Vector3{ aabb.max.x, aabb.min.y, aabb.min.z };
    points[2] = Vector3{ aabb.max.x, aabb.max.y, aabb.min.z };
    points[3] = Vector3{ aabb.min.x, aabb.max.y, aabb.min.z };
    points[4] = Vector3{ aabb.min.x, aabb.min.y, aabb.max.z };
    points[5] = Vector3{ aabb.max.x, aabb.min.y, aabb.max.z };
    points[6] = Vector3{ aabb.max.x, aabb.max.y, aabb.max.z };
    points[7] = Vector3{ aabb.min.x, aabb.max.y, aabb.max.z };

    // 6つの各平面に対してテスト
    for (const auto& plane : frustumPlanes_)
    {
        int inCount = 0;
        // AABBのすべての頂点が平面の裏側にあるかチェック
        for (int i = 0; i < 8; ++i)
        {
            float dist = plane.normal.Dot(points[i]) + plane.distance;
            if (dist >= 0)
            {
                inCount++;
            }
        }
        // すべての頂点が平面の裏側にある場合は、AABBは視錐台の外
        if (inCount == 0)
        {
            return false;
        }
    }

    return true; // どの平面の外側にもない場合は、視錐台内にあると判定
}

// 実際に動かす
void CameraController::MovingCenter()
{
    if (easeCenter_.maxFrame == 0)
    {
        easeCenter_.flame = 1;
        easeCenter_.maxFrame = 1;
    }
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    center_ = Easings::EasingVector3(easeCenter_.start, easeCenter_.end, easeCenter_.easetype, t);
    preCenter_ = center_;

    easeCenter_.flame++;
    if (easeCenter_.flame > easeCenter_.maxFrame)
    {
        easeCenter_.easingFlag = 0;
    }
}

void CameraController::MovingRotate()
{
    if (easeRotate_.maxFrame == 0)
    {
        easeRotate_.flame = 1;
        easeRotate_.maxFrame = 1;
    }
    float t = float(easeRotate_.flame) / float(easeRotate_.maxFrame);

    transform_.rotate = Easings::EasingVector3(easeRotate_.start, easeRotate_.end, easeRotate_.easetype, t);
    preRotate_ = transform_.rotate;

    easeRotate_.flame++;

    if (easeRotate_.flame > easeRotate_.maxFrame)
    {
        easeRotate_.easingFlag = 0;
    }
}

void CameraController::MovingDistance()
{
    if (easeDistance_.maxFrame == 0)
    {
        easeDistance_.flame = 1;
        easeDistance_.maxFrame = 1;
    }
    float t = float(easeDistance_.flame) / float(easeDistance_.maxFrame);

    distance_ = Easings::EasingFloat(easeDistance_.start.x, easeDistance_.end.x, easeDistance_.easetype, t);

    easeDistance_.flame++;

    if (easeDistance_.flame > easeDistance_.maxFrame)
    {
        easeDistance_.easingFlag = 0;
    }
}

// 動かす先の設定
void CameraController::SetCenterTarget(Vector3 target, int spendFrame, EaseType easetype)
{
    easeCenter_.start = center_;
    easeCenter_.end = target;
    easeCenter_.easingFlag = 1;
    easeCenter_.flame = 0;
    easeCenter_.maxFrame = spendFrame;
    easeCenter_.easetype = easetype;
};

void CameraController::SetRotateTarget(Vector3 target, int spendFrame, EaseType easetype)
{
    easeRotate_.start = transform_.rotate;
    easeRotate_.end = target;
    easeRotate_.easingFlag = 1;
    easeRotate_.flame = 0;
    easeRotate_.maxFrame = spendFrame;
    easeRotate_.easetype = easetype;
};

void CameraController::SetDistanceTarget(float target, int spendFrame, EaseType easetype)
{
    easeDistance_.start.x = distance_;
    easeDistance_.end.x = target;
    easeDistance_.easingFlag = 1;
    easeDistance_.flame = 0;
    easeDistance_.maxFrame = spendFrame;
    easeDistance_.easetype = easetype;
}

// シェイク
void CameraController::StartShake(float intensity, float duration, float frequency)
{
    shakeActive_ = true;
    shakeIntensity_ = intensity;
    shakeDuration_ = duration;
    shakeFrequency_ = frequency;
    shakeTime_ = 0.0f;
}

bool CameraController::IsShaking()
{
	return shakeActive_;
}

void CameraController::StopShake()
{
	shakeActive_ = false;
}

Vector3 CameraController::GetShakeOffset() const
{
    if (!shakeActive_) return Vector3(0.0f, 0.0f, 0.0f);

    // 正規化された時間 (0.0 から 1.0)
    float t = shakeTime_ / shakeDuration_;

    // 指数減衰 (時間とともに揺れが小さくなる)
    float decay = std::exp(-3.0f * t);

    // 揺れ計算（sin, cos の組み合わせで自然な揺れを作る）
    float offsetX = std::sin(shakeTime_ * shakeFrequency_) * shakeIntensity_ * decay;
    float offsetY = std::cos(shakeTime_ * shakeFrequency_ * 0.7f) * shakeIntensity_ * decay;
    float offsetZ = std::sin(shakeTime_ * shakeFrequency_ * 1.3f) * shakeIntensity_ * decay * 0.5f; // Z軸は控えめ

    return Vector3(offsetX, offsetY, offsetZ);
}
