#include "Camera/CameraController.h"
#include "Engine/Game.h"


CameraController::CameraController()
{
    mousePositionGap_ = { 0,0 };
    cameraMode_ = false;
     cameraMode_centerControl_ = true;
     cameraMode_rotateControl_ = true;
     cameraMode_distanceControl_ = true;
    cameraModeMode_ = true;

    // カメラ
    transform_.translate = { 0.0f, 0.0f, 0.0f };
    transform_.rotate = { 1.13f, 0.0f, 0.0f };
    center_ = { -11.390f, -0.170f, -5.530f };
    distance_ = 35.60f;

    preCenter_ = center_;
    preRotate_.x = transform_.rotate.x;
    preRotate_.y = transform_.rotate.y;

    sphereOptions.enableLighting = false;
}

void CameraController::Updata()
{
    // カメラ操作可能
    if (cameraMode_)
    {
        // 左クリック
        prePressMouse0_ = pressMouse0_;
        pressMouse0_ = Game::GetMousePress(0);
        // ミドルボタン
        prePressMouse2_ = pressMouse2_;
        pressMouse2_ = Game::GetMousePress(2);

        mouseWheel_ = Game::GetMouseWheel();

#pragma region カメラ回転
        if (cameraMode_rotateControl_)
        {
            // クリックした瞬間
            if (pressMouse2_ && prePressMouse2_ == 0 && !GetHitKey::keys[DIK_LSHIFT])
            {
                preMousePosition_ = Game::GetMousePosition();
            }
            // クリックしている最中
            if (pressMouse2_ && !GetHitKey::keys[DIK_LSHIFT])
            {
                mousePosition_ = Game::GetMousePosition();
                mousePositionGap_.x = mousePosition_.x - preMousePosition_.x;
                mousePositionGap_.y = mousePosition_.y - preMousePosition_.y;
                transform_.rotate.x = (mousePositionGap_.y / 100.0f) + (preRotate_.x);
                transform_.rotate.y = (mousePositionGap_.x / 100.0f) + (preRotate_.y);
            }
            // クリックやめた瞬間
            if (prePressMouse2_ && pressMouse2_ == 0 && !GetHitKey::keys[DIK_LSHIFT])
            {
                preRotate_ = transform_.rotate;
            }
        }
#pragma endregion

#pragma region 回転中心
        if (cameraMode_centerControl_)
        {
            if (prePressMouse2_ == 0 && pressMouse2_ && GetHitKey::keys[DIK_LSHIFT])
            {
                preMousePosition_ = Game::GetMousePosition();
            }
            if (pressMouse2_ && GetHitKey::keys[DIK_LSHIFT])
            {
                mousePosition_ = Game::GetMousePosition();
                mousePositionGap_.x = float(mousePosition_.x - preMousePosition_.x);
                mousePositionGap_.y = float(mousePosition_.y - preMousePosition_.y);

                //	カメラの回転行列（cameraRotMat）を作ることで、カメラの「右」「上」方向ベクトルを取得できます。
                //	右方向ベクトル = 回転行列の1列目（m[0][0], m[1][0], m[2][0]）
                //	上方向ベクトル = 回転行列の2列目（m[0][1], m[1][1], m[2][1]）
                //	前方向ベクトル = 回転行列の3列目（m[0][2], m[1][2], m[2][2]）
                //
                //      ↑ y（上）
                //      |
                //      |
                //      o----→ x（右）
                //     /
                //    /
                //    z（前）
                // 
                //　カメラの回転行列 は カメラの向いてる向き
                // 


                Matrix4x4 cameraRotMat = Matrix4x4::MakeAffineMatrix({ 1,1,1 }, transform_.rotate, { 0,0,0 });
                // 右方向ベクトル（ローカルx軸）
                Vector3 right = { cameraRotMat.m[0][0], cameraRotMat.m[1][0], cameraRotMat.m[2][0] };
                // 上方向ベクトル（ローカルy軸）
                Vector3 up = { cameraRotMat.m[0][1], cameraRotMat.m[1][1], cameraRotMat.m[2][1] };


                // Centerを移動
                //center_ = preCenter_ + (((right * -1) * (mousePositionGap_.x / 100.0f)) + ((up * -1) * (-mousePositionGap_.y / 100.0f)));
                center_ = preCenter_ + (((right * -1) * (mousePositionGap_.x / 100.0f)) + ((up * 1) * (-mousePositionGap_.y / 100.0f)));
            }
            if (prePressMouse2_ && pressMouse2_ == 0 && GetHitKey::keys[DIK_LSHIFT])
            {
                preCenter_ = center_;
            }
        }
#pragma endregion

#pragma region カメラ距離
        if (cameraMode_distanceControl_)
        {
            if (mouseWheel_ > 0)
            {
                distance_ -= float(mouseWheel_) / 100;
            }
            if (mouseWheel_ < 0)
            {
                distance_ -= float(mouseWheel_) / 100;
            }
        }

#pragma endregion
    }

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

#ifdef DEBUG
#endif
    ImGui::Begin("camera");
    ImGui::DragFloat3("cameraCenter", &center_.x, 0.01f);
    ImGui::DragFloat3("cameraRotate", &transform_.rotate.x, 0.01f);
    ImGui::DragFloat("cameraDistance", &distance_, 0.1f);
    ImGui::DragFloat3("cameratransform_.translate", &transform_.translate.x, 0.01f);
    ImGui::DragFloat3("cameratransform_.rotate", &transform_.rotate.x, 0.01f);
    ImGui::Text("push SPACE key : change cameraMode");
    ImGui::Checkbox("cameraMode", &cameraMode_);
    ImGui::Checkbox("cameraModeMode", &cameraModeMode_);
    ImGui::End();

    //////////////////////////////////////////////
    ///               カメラ移動               ///
    ////////////////////////////////////////////// 

    if (cameraModeMode_)
    {
        ////  カメラを原点で回転させた後に移動  ////（カメラのscaleとtranslateは動かさない）

        // カメラ初期値
        Vector3 cameraLocalPos = { 0.0f, 0.0f, -distance_ };

        // カメラに回転適用（カメラのscaleとtranslateはマジで動かさない）
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
        transform_.translate = (center_ + rotatedCameraPos);
        //transform_.translate = cameraLocalPos;
    }
    else
    {
        Vector3 cameraLocalPos = { center_.x - transform_.translate.x, center_.y - transform_.translate.y, center_.z - transform_.translate.z };

        // カメラに回転適用（カメラのscaleとtranslateはマジで動かさない）
        Matrix4x4 cameraRotateMatrix = Matrix4x4::MakeAffineMatrix(
            { 1,1,1 },
            transform_.rotate,
            { 0,0,0 }
        );

        // cameraLocalPos　に　回転適用したマトリックスを適用させる　＝　カメラの座標で回転
        Vector3 rotatedCameraPos = {
            cameraLocalPos.x * cameraRotateMatrix.m[0][0] + cameraLocalPos.y * cameraRotateMatrix.m[1][0] + cameraLocalPos.z * cameraRotateMatrix.m[2][0],
            cameraLocalPos.x * cameraRotateMatrix.m[0][1] + cameraLocalPos.y * cameraRotateMatrix.m[1][1] + cameraLocalPos.z * cameraRotateMatrix.m[2][1],
            cameraLocalPos.x * cameraRotateMatrix.m[0][2] + cameraLocalPos.y * cameraRotateMatrix.m[1][2] + cameraLocalPos.z * cameraRotateMatrix.m[2][2]
        };


        // 原点上で回転したrotatedCameraPosに　cameraCenterを足せば中心がcameraCenterに変わる
        center_ = rotatedCameraPos + transform_.translate;
    }

    // カメラ行列を作成
    cameraMatrix_ = Matrix4x4::MakeAffineMatrix(
        { 1,1,1 },
        transform_.rotate,
        transform_.translate
    );

    // ビュー・射影・ビューポート行列
    viewMatrix_ = (cameraMatrix_.Inverse());
    projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
    viewProjectionMatrix = (viewMatrix_ * projectionMatrix_);
}

void CameraController::Draw()
{
    Game::DrawSphere({ {0.1f,0.1f,0.1f}, {0.0f,0.0f,0.0f}, center_ }, { 0,0,0 }, 12, 0, 0xFFFFFFFF, sphereOptions);
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

    center_ = Easings::EasingVector3(easeCenter_.start, easeCenter_.end, EaseType::OUT_QUART, t);
    preCenter_ = center_;

    easeCenter_.flame++;
    if (easeCenter_.flame > easeCenter_.maxFrame)
    {
        easeCenter_.easingFlag = 0;
    }
}

void CameraController::MovingRotate()
{
    if (easeCenter_.maxFrame == 0)
    {
        easeCenter_.flame = 1;
        easeCenter_.maxFrame = 1;
    }
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    transform_.rotate = Easings::EasingVector3(easeRotate_.start, easeRotate_.end, EaseType::OUT_QUART, t);
    preRotate_ = transform_.rotate;

    easeRotate_.flame++;

    if (easeRotate_.flame > easeRotate_.maxFrame)
    {
        easeRotate_.easingFlag = 0;
    }
}

void CameraController::MovingDistance()
{
    if (easeCenter_.maxFrame == 0)
    {
        easeCenter_.flame = 1;
        easeCenter_.maxFrame = 1;
    }
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    distance_ = Easings::EasingFloat(easeDistance_.start.x, easeDistance_.end.x, EaseType::OUT_QUART, t);

    easeDistance_.flame++;

    if (easeDistance_.flame > easeDistance_.maxFrame)
    {
        easeDistance_.easingFlag = 0;
    }
}

// 動かす先の設定
void CameraController::SetCenterTarget(Vector3 target, int spendFrame)
{
    easeCenter_.start = center_;
    easeCenter_.end = target;
    easeCenter_.easingFlag = 1;
    easeCenter_.flame = 0;
    easeCenter_.maxFrame = spendFrame;
};

void CameraController::SetRotateTarget(Vector3 target, int spendFrame)
{
    easeRotate_.start = transform_.rotate;
    easeRotate_.end = target;
    easeRotate_.easingFlag = 1;
    easeRotate_.flame = 0;
    easeRotate_.maxFrame = spendFrame;
};

void CameraController::SetDistanceTarget(float target, int spendFrame)
{
    easeDistance_.start.x = distance_;
    easeDistance_.end.x = target;
    easeDistance_.easingFlag = 1;
    easeDistance_.flame = 0;
    easeDistance_.maxFrame = spendFrame;
};
