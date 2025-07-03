#include "Camera/CameraController.h"
#include "Engine/Game.h"
Matrix4x4 CameraController::viewportMatrix;
Matrix4x4 CameraController::viewProjectionMatrix;


CameraController::CameraController()
{
    mousePositionGap_ = { 0,0 };
    cameraMode_ = 1;

    // カメラ
    transform_.translate = { 0.0f, 0.0f, 0.0f };
    transform_.rotate = { 0.43f, 0.0f, 0.0f };
    center_ = { 0.0f, 0.0f, 0.0f };
    distance_ = 39.60f;

    preCenter_ = center_;
    preRotate_.x = transform_.rotate.x;
    preRotate_.y = transform_.rotate.y;

    rotateTime_ = false;
    centerTime_ = false;

    sphereOptions.enableLighting = false;
}

void CameraController::Updata()
{
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

    // カメラ行列を作成
    cameraMatrix_ = Matrix4x4::MakeAffineMatrix(
        { 1,1,1 },
        transform_.rotate,
        transform_.translate
    );

    // ビュー・射影・ビューポート行列
    viewMatrix_ = (cameraMatrix_.Inverse());



    // カメラ操作可能
    if (cameraMode_)
    {
        // 左クリック
        prePressMouse0_ = pressMouse0_;
        pressMouse0_ = Game::IsPressMouse(0);
        // ミドルボタン
        prePressMouse2_ = pressMouse2_;
        pressMouse2_ = Game::IsPressMouse(2);

        mouseWheel_ = Game::GetWheel();

        if (prePressMouse2_ == 0 && pressMouse2_)
        {
            Game::GetMousePosition(&preMousePosition_);
        }
        if (pressMouse2_)
        {
            Game::GetMousePosition(&mousePosition_);
            if (GetHitKey::keys[DIK_LSHIFT])
            {
                centerTime_ = true;
                rotateTime_ = false;
            }
            else
            {
                centerTime_ = false;
                rotateTime_ = true;
            }
        }


        //////////////////////////////////////////////
        ///              カメラ回転                ///
        ////////////////////////////////////////////// 
#pragma region
        if (rotateTime_)
        {
            // クリックしている最中
            if (pressMouse2_)
            {
                mousePositionGap_.x = mousePosition_.x - preMousePosition_.x;
                mousePositionGap_.y = mousePosition_.y - preMousePosition_.y;
                transform_.rotate.x = (mousePositionGap_.y / 100.0f) + (preRotate_.x);
                transform_.rotate.y = (mousePositionGap_.x / 100.0f) + (preRotate_.y);
            }
            // クリックやめた瞬間
            if (prePressMouse2_ && pressMouse2_ == 0)
            {
                preRotate_ = transform_.rotate;
            }
        }

#pragma endregion

        //////////////////////////////////////////////
        ///                回転中心                ///
        ////////////////////////////////////////////// 
#pragma region
        if (centerTime_)
        {
            if (pressMouse2_)
            {
                mousePositionGap_.x = float(mousePosition_.x - preMousePosition_.x);
                mousePositionGap_.y = float(mousePosition_.y - preMousePosition_.y);

                // カメラのビュー行列の逆行列を取得
                // viewMatrix_はCameraController::Updata()の最後で計算されているので、それが最新の値であることを確認してください。
                // もしviewMatrix_がまだ更新されていない段階でこの処理が実行されると、不正な結果になります。
                // 安全のため、viewMatrix_はここで一度計算し直すか、Updateの最後に計算されることを前提とします。
                // ここでは、Update()の最後に計算されたviewMatrix_が最新であると仮定します。
                Matrix4x4 invViewMatrix = viewMatrix_.Inverse();

                // ワールド空間におけるカメラの「右」方向ベクトル
                Vector3 worldRight = { invViewMatrix.m[0][0], invViewMatrix.m[1][0], invViewMatrix.m[2][0] };
                // ワールド空間におけるカメラの「上」方向ベクトル
                Vector3 worldUp = { invViewMatrix.m[0][1], invViewMatrix.m[1][1], invViewMatrix.m[2][1] };

                float winHeight = 720.0f; // ウィンドウの高さは固定値でOK

                // パンのスケール係数。現在の距離とFOVから、1ピクセルあたりのワールド空間での移動量を計算
                // 0.45f は垂直FOV (radian)
                float panScale = distance_ * tanf(0.45f / 2.0f) * 2.0f / winHeight;

                // Centerを移動
                // マウスのX移動はワールドのRight方向に、Y移動はワールドのUp方向にマッピング
                // マウスのY方向は画面下向きが正として、カメラの上方向と逆になるように調整
                center_ = preCenter_ + (worldRight * (-mousePositionGap_.x * panScale)) + (worldUp * (-mousePositionGap_.y * panScale));
                // ※ ここで -mousePositionGap_.y としているのは、一般的なマウス操作（上に動かすとビューも上にパン）に合わせるためです。
                //    もしマウスを上に動かしたときにmousePositionGap_.yが負の値になるなら、
                //    `worldUp * (mousePositionGap_.y * panScale)` とすることで期待通りになる場合もあります。
                //    実際の挙動を確認して調整してください。
            }
            if (prePressMouse2_ && pressMouse2_ == 0)
            {
                preCenter_ = center_;
            }
        }
#pragma endregion

        //////////////////////////////////////////////
        ///               カメラ距離               ///
        ////////////////////////////////////////////// 
#pragma region
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
    ImGui::DragFloat3("cameraCenter", &center_.x, 0.01f);
    ImGui::DragFloat3("cameraRotate", &transform_.rotate.x, 0.01f);
    ImGui::DragFloat("cameraDistance", &distance_, 0.01f);
#endif
    ImGui::Text("push SPACE key : change cameraMode");
    ImGui::Checkbox("cameraMode", &cameraMode_);

    //////////////////////////////////////////////
    ///               カメラ移動               ///
    ////////////////////////////////////////////// 

    ////  カメラを原点で回転させた後に移動  ////（カメラのscaleとtranslateは動かさない）

    // カメラ初期値

    projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
    viewProjectionMatrix = (viewMatrix_ * projectionMatrix_);

}

void CameraController::Draw()
{
    //Game::DrawSphere({ {0.1f,0.1f,0.1f}, {0.0f,0.0f,0.0f}, center_ }, { 0,0,0 }, 12, 0, 0xFFFFFFFF, sphereOptions);


}

// 実際に動かす
void CameraController::MovingCenter()
{
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    center_.x = Easings::OUT_QUART(easeCenter_.start.x, easeCenter_.end.x, t);
    center_.y = Easings::OUT_QUART(easeCenter_.start.y, easeCenter_.end.y, t);
    center_.z = Easings::OUT_QUART(easeCenter_.start.z, easeCenter_.end.z, t);
    preCenter_ = center_;

    easeCenter_.flame++;
    if (easeCenter_.flame > easeCenter_.maxFrame)
    {
        easeCenter_.easingFlag = 0;
    }
}

void CameraController::MovingRotate()
{
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    center_.x = Easings::OUT_QUART(easeRotate_.start.x, easeRotate_.end.x, t);
    center_.y = Easings::OUT_QUART(easeRotate_.start.y, easeRotate_.end.y, t);
    center_.z = Easings::OUT_QUART(easeRotate_.start.z, easeRotate_.end.z, t);
    preRotate_ = transform_.rotate;

    easeRotate_.flame++;

    if (easeRotate_.flame > easeRotate_.maxFrame)
    {
        easeRotate_.easingFlag = 0;
    }
}

void CameraController::MovingDistance()
{
    float t = float(easeCenter_.flame) / float(easeCenter_.maxFrame);

    distance_ = Easings::OUT_QUART(easeDistance_.start.x, easeDistance_.end.x, t);

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
