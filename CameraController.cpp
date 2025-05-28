#include "CameraController.h"
#include "functions.h"
#include "Game.h"
Matrix4x4 CameraController::viewportMatrix;
Matrix4x4 CameraController::viewProjectionMatrix;


CameraController::CameraController()
{
    transform.translate = { 0.0f,1.9f,-6.49f };
    transform.rotate = { 0.26f,0.0f,0.0f };
    mousePositionGap = { 0,0 };
    cameraMode = 1;
}

void CameraController::Updata()
{
    if (cameraMode)
    {
        prePressMouse0 = pressMouse0;
        pressMouse0 = Game::IsPressMouse(0);
        prePressMouse2 = pressMouse2;
        pressMouse2 = Game::IsPressMouse(2);

        mouseWheel = Game::GetWheel();

        //////////////////////////////////////////////
        ///              カメラ回転                ///
        ////////////////////////////////////////////// 
        // クリックした瞬間
        if (prePressMouse0 == 0 && pressMouse0)
        {
            Game::GetMousePosition(&preMousePosition);
        }
        // クリックしている最中
        if (pressMouse0)
        {
            Game::GetMousePosition(&mousePosition);
            mousePositionGap.x = mousePosition.x - preMousePosition.x;
            mousePositionGap.y = mousePosition.y - preMousePosition.y;
            transform.rotate.x = (mousePositionGap.y / 100.0f + 0.26f) + (preRotate.x);
            transform.rotate.y = (mousePositionGap.x / 100.0f) + (preRotate.y);
        }
        // クリックやめた瞬間
        if (prePressMouse0 && pressMouse0 == 0)
        {
            preRotate.x = transform.rotate.x;
            preRotate.y = transform.rotate.y;
        }

        //////////////////////////////////////////////
        ///                回転中心                ///
        ////////////////////////////////////////////// 
        if (prePressMouse2 == 0 && pressMouse2)
        {
            Game::GetMousePosition(&preMousePosition);
        }
        if (pressMouse2)
        {
            Game::GetMousePosition(&mousePosition);
            mousePositionGap.x = float(mousePosition.x - preMousePosition.x);
            mousePositionGap.y = float(mousePosition.y - preMousePosition.y);

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


            Matrix4x4 cameraRotMat = MakeAffineMatrix({ 1,1,1 }, transform.rotate, { 0,0,0 });
            // 右方向ベクトル（ローカルx軸）
            Vector3 right = { cameraRotMat.m[0][0], cameraRotMat.m[1][0], cameraRotMat.m[2][0] };
            // 上方向ベクトル（ローカルy軸）
            Vector3 up = { cameraRotMat.m[0][1], cameraRotMat.m[1][1], cameraRotMat.m[2][1] };


            // Targetを移動
            target = Add(preTarget, Add(
                Mul(mousePositionGap.x / 100.0f, Mul(-1, right)),
                Mul(-mousePositionGap.y / 100.0f, Mul(-1, up))
            ));
        }
        if (prePressMouse2 && pressMouse2 == 0)
        {
            preTarget = target;
        }



        //////////////////////////////////////////////
        ///               カメラ距離               ///
        ////////////////////////////////////////////// 
        if (mouseWheel > 0)
        {
            distance -= float(mouseWheel) / 100;
        }
        if (mouseWheel < 0)
        { 
            distance -= float(mouseWheel) / 100;
        }
    }

    //////////////////////////////////////////////
    ///               カメラ移動               ///
    ////////////////////////////////////////////// 

    ////  カメラを原点で回転させた後に移動  ////（カメラのscaleとtranslateは動かさない）

    // カメラ初期値
    Vector3 cameraLocalPos = { 0.0f, 0.0f, -distance };

    // カメラに回転適用（カメラのscaleとtranslateはマジで動かさない）
    Matrix4x4 cameraRotateMatrix = MakeAffineMatrix(
        { 1,1,1 },
        transform.rotate,
        { 0,0,0 }
    );

    // cameraLocalPos　に　回転適用したマトリックスを適用させる　＝　原点上で回転
    Vector3 rotatedCameraPos = {
        cameraLocalPos.x * cameraRotateMatrix.m[0][0] + cameraLocalPos.y * cameraRotateMatrix.m[1][0] + cameraLocalPos.z * cameraRotateMatrix.m[2][0],
        cameraLocalPos.x * cameraRotateMatrix.m[0][1] + cameraLocalPos.y * cameraRotateMatrix.m[1][1] + cameraLocalPos.z * cameraRotateMatrix.m[2][1],
        cameraLocalPos.x * cameraRotateMatrix.m[0][2] + cameraLocalPos.y * cameraRotateMatrix.m[1][2] + cameraLocalPos.z * cameraRotateMatrix.m[2][2]
    };


    // 原点上で回転したrotatedCameraPosに　cameraTargetを足せば中心がcameraTargetに変わる
    transform.translate = Add(target, rotatedCameraPos);

    // カメラ行列を作成
    cameraMatrix = MakeAffineMatrix(
        { 1,1,1 },
        transform.rotate,
        transform.translate
    );

    // ビュー・射影・ビューポート行列
    viewMatrix = Inverse(cameraMatrix);
    projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
    viewProjectionMatrix = Mul(viewMatrix, projectionMatrix);
    //viewportMatrix = MakeViewPortMatrix(0, 0, float(1280), float(720), 0.0f, 1.0f);

    //cameraMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    //viewMatrix = Inverse(cameraMatrix);
    //projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);// int width, int heightをもってくる



}

void CameraController::Draw()
{
    //Game::DrawSphere(transformSphere1, vertexData, kSubdivision, monsterBall, { 1.0f, 1.0f, 1.0f, 1.0f });
}
