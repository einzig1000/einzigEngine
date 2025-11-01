#include "CameraManager.h"
#include "Camera.h"

CameraManager::CameraManager()
{
	Camera ReleaseCamera;
	ReleaseCamera.name_ = "ReleaseCamera";
	camera_.push_back(ReleaseCamera);

	Camera DebugCamera;
	DebugCamera.name_ = "DebugCamera";
	camera_.push_back(DebugCamera);


}

CameraManager::~CameraManager()
{
}

void CameraManager::Update()
{
	camera_[currentCameraID_].Update();
}

void CameraManager::Draw()
{
	camera_[currentCameraID_].Draw();
}

void CameraManager::Resize()
{
	for (auto& cam : camera_)
	{
		cam.Resize();
	}
}

void CameraManager::SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetCenterTarget(Center, spendFrame, easetype);
}

void CameraManager::SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetRotateTarget(Center, spendFrame, easetype);
}

void CameraManager::SetDistanceTarget(float Center, int spendFrame, EaseType easetype)
{
	camera_[currentCameraID_].SetDistanceTarget(Center, spendFrame, easetype);
}

void CameraManager::StartShake(float intensity, float duration, float frequency)
{
	camera_[currentCameraID_].StartShake(intensity, duration, frequency);
}

bool CameraManager::IsShaking()
{
	return camera_[currentCameraID_].IsShaking();
}

void CameraManager::StopShake()
{
	camera_[currentCameraID_].StopShake();
}

Vector3 CameraManager::GetCenter() const
{
	return camera_[currentCameraID_].GetCenter();
}

Vector3 CameraManager::GetTranslate() const
{
	return camera_[currentCameraID_].GetTranslate();
}

Matrix4x4 CameraManager::GetViewProjectionMatrix() const
{
	return camera_[currentCameraID_].GetViewProjectionMatrix();
}

float CameraManager::GetDistance() const
{
	return camera_[currentCameraID_].GetDistance();
}

bool CameraManager::InCamera(const AABB& aabb)
{
	return camera_[currentCameraID_].InFrustum(aabb);
}

